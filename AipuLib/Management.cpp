#include "Management.h"

Management::Management()
{
	SetDirectoryConfiguration();
	ObserverError();
	
	ObserverTemplateImage();
	ObserverIdentifyFace();
	ObserverDatabase();
}

Management::~Management()
{
}

void Management::SetDirectoryConfiguration() {
	faceModel->configuration->SetNameDirectory(DIRECTORY_CONFIGURATION);
	//video->configuration->SetNameDirectory(DIRECTORY_CONFIGURATION);
	configurationFile->SetNameDirectory(DIRECTORY_CONFIGURATION);
	database->configuration->SetNameDirectory(DIRECTORY_CONFIGURATION);
}

void Management::ObserverIdentifyFace() {
	auto identifyUser = identify->observableUser.map([](User* user) {
		return user;
	});

	auto subscriptionIdentifyUser = identifyUser.subscribe([this](User* user) {
		
		if (user->GetIsNew())
		{			
			string number = to_string(user->GetUserIdIFace());
			string name = "Person " + number;
			string lastName = "LasName " + number;
			string identification = "0000000";
			user->SetNameUser(name);
			user->SetLastNameUser(lastName);
			user->SetIdentificationUser(identification);
			database->InsertNewUser(user);
		}
		else {
			database->FindUserByIdFace(user->GetUserIdIFace(), 
				user->GetPathImageTemp(), user->GetClient());
		}
		
	});

}

void Management::ObserverDatabase() {
	auto databaseObservable = database->observableUserJSON.map([](string jsonUser) {
		return jsonUser;
	});

	auto subscriptionDatabase = databaseObservable.subscribe([this](string jsonUser) {
		
		shootUserJSON.on_next(jsonUser);
	});

}

void Management::ObserverTemplateImage()
{
	auto templateObservable = faceModel->observableTemplate.map([](Molded* modelImage) {
		return modelImage;
	});

	auto subscriptionTemplate = templateObservable.subscribe([this](Molded* modelImage) {
		identify->EnrollUser(modelImage);
	});
	
}

void Management::ObserverError() {

	auto faceModelError = faceModel->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionFaceModelError = faceModelError.subscribe([this](Either* either) {	
		if (either->GetLabel() != "OK")
		{
			shootError.on_next(either);
		}
		
	});

	/*auto videoError = video->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionVideoError = videoError.subscribe([this](Either* either) {		
		shootError.on_next(either);
	});*/

	auto flowTrendError = flowTrend->observableError.map([](Either* either) {		
		return either;
	});

	auto subscriptionFlowTrendError = flowTrendError.subscribe([this](Either* either) {		
		shootError.on_next(either);
	});

	auto configurationFileError = configurationFile->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionConfigurationFileError = configurationFileError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});

	auto identifyError = identify->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionIdentifyError = identifyError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});

	auto databaseError = database->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionDatabaseError = databaseError.subscribe([this](Either* either) {
		shootError.on_next(either);
	});
}

void Management::SaveDataTraining(int quantityDetected, int day, int hour, int minute) {	
	string fileName = to_string(day) + "-" + to_string(hour) + ".txt";	
	train->SetMinute(minute);
	train->SetQuantityOfImages(quantityDetected);
	train->WriteDataTraining(fileName);

}

void Management::VerifyTrainingLapse() {
	std::chrono::steady_clock::time_point endCountTime = std::chrono::steady_clock::now();
	int diff = std::chrono::duration_cast<std::chrono::minutes>(endCountTime - startCountTime).count();
	if (diff >= ONE_MINUTE)
	{		
		std::time_t rawtime = std::time(nullptr);		
		tm *timeinfo = std::localtime(&rawtime);
		int day = timeinfo->tm_wday;
		int hour = timeinfo->tm_hour;
		int min = (hour * SIXTY_MINUTES) + timeinfo->tm_min;
		std::thread(&Management::SaveDataTraining, this,
			countImagesDetected, day, hour, min).detach();
		startCountTime = std::chrono::steady_clock::now();
		countImagesDetected = 0;		
		workMode = SetStateFlow(min + ONE_MINUTE);

	}

}

int Management::SetStateFlow(int minute) {	
	int valueTrend = flowTrend->GetFlowTrendForMinute(minute);
	cout << "VALUE TREND: " << valueTrend << endl;
	
	
	if (timeDurationSingleDetection != 0.0)
	{
		int imagesWait = static_cast<int>(
			(SIXTY_SECONDS / timeDurationSingleDetection) / 3);
		
		if (valueTrend > imagesWait)
		{

			return SINGLE;
		}
		if (valueTrend <= imagesWait && valueTrend != 0)
		{
			return OPTION_BATCH;
		}
		else
		{
			return MOVEMENT;
		}

	}
	return SINGLE;

}


void Management::GetModelOneToOne(Mat image, int client) {
	vector<uchar> buffer = video->WriteImageOnBuffer(image);	

	chrono::steady_clock sc;
	auto start = sc.now();
	int count = faceModel->ModelOneToOne(buffer, client);
	if (count != 0)
	{
		auto end = sc.now();
		auto time_span = static_cast<chrono::duration<double>>(end - start);
		timeDurationSingleDetection = time_span.count();

	}
	buffer.clear();
	countImagesDetected += count;

}

void Management::GetModelsByBatch(Mat image, int client) {
	video->WriteBatchOfImagesOnBuffer(image, client);
	if (video->GetProcessedImages() == SIZE_BUFFER) {
		std::vector<std::vector<uchar>> bufferImages = video->GetBufferOfImages();
		std::vector<int> clients = video->GetBufferOfClients();
		countImagesDetected += faceModel->ModelByBatch(bufferImages, clients);
		video->ClearBufferImagesBatch();
		video->ClearBufferClientsBatch();
	}

}

void Management::ProcessImage(Mat image, int client) {
	VerifyTrainingLapse();

	switch (workMode)
	{
	case SINGLE:
		
		GetModelOneToOne(image, client);
		cout << "SINGLE DETECTION" << endl;
		break;
	case OPTION_BATCH:
		GetModelsByBatch(image, client);
		cout << "BATCH DETECTION" << endl;
		break;
	case MOVEMENT:
		if (movement->SetImage(image))
		{
			if (movement->IsThereMovement()) {
				workMode = SINGLE;
			}
			cout << "MOVEMENT DETECTION" << endl;
		}
		break;
	default:
		break;
	}

	flagNextFrame = false;

}

void Management::RecognitionFace(unsigned char* image, 
	int rows, int cols, int client) {
	
	if (!flagNextFrame) {
		flagNextFrame = true;
		Mat matImage = ByteToMat(image, rows, cols);
		
		std::thread squ(&Management::ProcessImage, this, matImage, client);
		squ.detach();

	}
}

Mat Management::ByteToMat(unsigned char* image, int rows, int cols) {
	Mat matImage = Mat(rows, cols, CV_8UC3);
	matImage.data = image;
	Mat reverseImage;
	bitwise_not(matImage, reverseImage);
	//imwrite("147215.png", reverseImage);
	return reverseImage;
}

