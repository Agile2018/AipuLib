#include "FaceModel.h"

FaceModel::FaceModel()
{
	ObserverError();
}

FaceModel::~FaceModel()
{
	Terminate();
}

void FaceModel::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {
		
		shootError.on_next(either);
	});

	auto observerErrorConfiguration = configuration->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionErrorConfiguration = observerErrorConfiguration.subscribe([this](Either* either) {
		
		shootError.on_next(either);
	});

}

void FaceModel::Terminate() {
	int errorCode = IFACE_Terminate();
	error->CheckError(errorCode, error->less);

}

int FaceModel::DetectByBatch(void* facesDetected[BATCH_TOTAL_SIZE],
	std::vector<std::vector<unsigned char>> bufferOfImagesBatch, 
	std::vector<int> clients) {
	int errorCode, countFacesDetected = 0;
	void* faceHandler;

	srand((unsigned)time(NULL));
	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(faceHandler,
		IFACE_PARAMETER_FACEDET_SPEED_ACCURACY_MODE,
		IFACE_FACEDET_SPEED_ACCURACY_MODE_BALANCED);
	error->CheckError(errorCode, error->medium);

	for (int i = 0; i < BATCH_TOTAL_SIZE; i++)
	{

		errorCode = IFACE_CreateFace(&(facesDetected[i]));
		error->CheckError(errorCode, error->medium);
	}

	for (int i = 0; i < BATCH_SIZE; i++)
	{
		int lenght, widthFace, heightFace;

		vector<unsigned char> buffer = bufferOfImagesBatch[i];
		const char* imgData = reinterpret_cast<const char*> (&buffer[0]);

		if (imgData != NULL) {
			errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &widthFace,
				&heightFace, &lenght, NULL);
			if (errorCode == IFACE_OK)
			{
				int maxFaces = configuration->GetMaxDetect();
				unsigned char* rawImage = new unsigned char[lenght];
				errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &widthFace,
					&heightFace, &lenght, rawImage);

				error->CheckError(errorCode, error->medium);
				void** faceTemp = new void*[maxFaces];

				for (int j = 0; j < maxFaces; j++) {
					errorCode = IFACE_CreateFace(&(faceTemp[j]));
					error->CheckError(errorCode, error->medium);
				}

				int detectedFaces = configuration->GetMaxDetect();
		
				errorCode = IFACE_DetectFaces(rawImage, widthFace, heightFace,
					configuration->GetMinEyeDistance(), configuration->GetMaxEyeDistance(),
					faceHandler, &detectedFaces, faceTemp);
				delete rawImage;
				error->CheckError(errorCode, error->medium);

				if (detectedFaces != EMPTY_FACE)
				{
					for (int j = 0; j < detectedFaces; j++) {
						float rightEyeX, rightEyeY, leftEyeX, leftEyeY;
						float faceConfidence;
						void* face = faceTemp[j];
						errorCode = IFACE_GetFaceBasicInfo(face, faceHandler,
							&rightEyeX, &rightEyeY, &leftEyeX, &leftEyeY, &faceConfidence);
						error->CheckError(errorCode, error->medium);
						cout << "CONFIDENCE: " << faceConfidence << endl;
						if (faceConfidence > configuration->GetPrecision() && 
							countFacesDetected < BATCH_TOTAL_SIZE)
						{
							errorCode = IFACE_CloneEntity(face, facesDetected[countFacesDetected]);
							error->CheckError(errorCode, error->medium);
							int randomName = rand() % 100 + 1;
							string pathImage = nameDirectory + "/" + to_string(randomName) + ".png";
							cout << "GREATER OR EQUAL ACCURACY .." << configuration->GetPrecision() << endl;
							std::thread(&FaceModel::FaceCropImage, this,
								facesDetected[countFacesDetected], pathImage).detach();
							pathCropImages.push_back(pathImage);
							clientsId.push_back(clients[i]);
							countFacesDetected++;
						}
					}

				}

				for (int j = 0; j < maxFaces; j++) {
					errorCode = IFACE_ReleaseEntity(faceTemp[j]);
					error->CheckError(errorCode, error->medium);
				}
				
			}
			else {
				error->CheckError(errorCode, error->medium);
			}

		}


	}
	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);

	return countFacesDetected;
}

void FaceModel::FaceCropImage(void* face, string pathImage) {
	int cropWidth, cropHeight, cropLength, errorCode;
	void* faceHandler;

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);


	errorCode = IFACE_GetFaceCropImage(face, faceHandler, IFACE_FACE_CROP_METHOD_FULL_FRONTAL, 
		&cropWidth, &cropHeight, &cropLength, NULL);
	error->CheckError(errorCode, error->medium);
	unsigned char* cropImageData = new unsigned char[cropLength];
	errorCode = IFACE_GetFaceCropImage(face, faceHandler, IFACE_FACE_CROP_METHOD_FULL_FRONTAL, 
		&cropWidth, &cropHeight, &cropLength, cropImageData);
	error->CheckError(errorCode, error->medium);
		
	if (errorCode == IFACE_OK) {
		errorCode = IFACE_SaveImage(pathImage.c_str(), cropWidth, cropHeight, 3, cropImageData);
	}
	error->CheckError(errorCode, error->medium);
			
	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);
	delete[] cropImageData;
}

int FaceModel::ModelByBatch(std::vector<std::vector<unsigned char>> bufferOfImagesBatch, 
	std::vector<int> clients) {
	int errorCode;
	void* facesDetected[BATCH_TOTAL_SIZE];
	int countFacesDetected = DetectByBatch(facesDetected, bufferOfImagesBatch, clients);
	if (countFacesDetected != 0)
	{
		GetBatchModels(countFacesDetected, facesDetected);
	}	

	for (int i = 0; i < BATCH_TOTAL_SIZE; i++)
	{
		errorCode = IFACE_ReleaseEntity(facesDetected[i]);
		error->CheckError(errorCode, error->medium);
	}
	return countFacesDetected;
}

void FaceModel::GetBatchModels(int countFacesDetected, void* facesDetected[BATCH_TOTAL_SIZE]) {
	int errorCode;
	int templateBatchDataSize;
	void** cloneFacesDetected = new void*[countFacesDetected];
	char** batchTemplates = new char*[countFacesDetected];
	void* faceHandler;

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);

	for (int i = 0; i < countFacesDetected; i++)
	{
		errorCode = IFACE_CreateFace(&(cloneFacesDetected[i]));
		error->CheckError(errorCode, error->medium);
	}

	for (int i = 0; i < countFacesDetected; i++)
	{
		errorCode = IFACE_CloneEntity(facesDetected[i], cloneFacesDetected[i]);
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_CreateTemplateBatch(faceHandler, countFacesDetected, cloneFacesDetected,
		&templateBatchDataSize, NULL);
	error->CheckError(errorCode, error->medium);

	for (int i = 0; i < countFacesDetected; ++i)
	{

		batchTemplates[i] = new char[templateBatchDataSize];
	}

	errorCode = IFACE_CreateTemplateBatch(faceHandler, countFacesDetected, cloneFacesDetected,
		&templateBatchDataSize, batchTemplates);

	error->CheckError(errorCode, error->medium);
	
	for (int i = 0; i < countFacesDetected; i++)
	{
		//string pathImage = nameDirectory + "/" + to_string(countFacesDetected) + nameFileCropImage;
		Molded* model = new Molded();
		model->SetMoldImage(batchTemplates[i]);
		model->SetMoldSize(templateBatchDataSize);
		model->SetPathImage(pathCropImages[i]);
		model->SetIdClient(clientsId[i]);
		templateOut.on_next(model);
		
	}	

	for (int i = 0; i < countFacesDetected; ++i)
	{
		delete batchTemplates[i];
	}
	for (int i = 0; i < countFacesDetected; i++)
	{
		errorCode = IFACE_ReleaseEntity(cloneFacesDetected[i]);
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);
	pathCropImages.clear();
	clientsId.clear();
}


int FaceModel::ModelOneToOne(vector<unsigned char> buffer, int client) {
	int lenght, width, height, errorCode, templates;
	const char* imgData = reinterpret_cast<const char*> (&buffer[0]);

	if (imgData != NULL) {
		errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &width,
			&height, &lenght, NULL);
		if (errorCode == IFACE_OK)
		{
			
			unsigned char* rawImage = new unsigned char[lenght];
			errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &width,
				&height, &lenght, rawImage);
			if (errorCode != IFACE_OK)
			{
				error->CheckError(errorCode, error->medium);
			}
			else
			{
				templates = GetOneModel(rawImage, width, height, client);
			}
			delete rawImage;
		}
		else {
			error->CheckError(errorCode, error->medium);
		}
	}

	return templates;
}


void FaceModel::CreateTemplate(void* face, string pathImageCrop, int client) {
	int errorCode;
	int templateSize;
	void* faceHandler;	

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_CreateTemplate(face, faceHandler, 0, &templateSize, NULL);
	if (errorCode == IFACE_OK) {
		char* templateData = new char[templateSize];
		errorCode = IFACE_CreateTemplate(face, faceHandler, 0,
			&templateSize, templateData);
		if (errorCode != IFACE_OK) {
			error->CheckError(errorCode, error->medium);
		}
		else
		{
			//string pathImage = nameDirectory + "/" + nameFileCropImage;
			Molded* model = new Molded();
			model->SetMoldImage(templateData);
			model->SetMoldSize(templateSize);
			model->SetPathImage(pathImageCrop);
			model->SetIdClient(client);
			templateOut.on_next(model);
		}
		delete[] templateData;
	}
	else {
		error->CheckError(errorCode, error->medium);
	}
	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);

}

int FaceModel::GetOneModel(unsigned char* rawImage, 
	int width, int height, int client) {
	int maxFaces = configuration->GetMaxDetect();
	int detectedFaces = configuration->GetMaxDetect();
	int errorCode;
	void** faceTemp = new void*[maxFaces];
	void* faceHandler;

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(faceHandler, 
		IFACE_PARAMETER_FACEDET_SPEED_ACCURACY_MODE, 
		IFACE_FACEDET_SPEED_ACCURACY_MODE_BALANCED);
	error->CheckError(errorCode, error->medium);

	for (int i = 0; i < maxFaces; i++) { 
		errorCode = IFACE_CreateFace(&(faceTemp[i]));
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_DetectFaces(rawImage, width, height,
		configuration->GetMinEyeDistance(), configuration->GetMaxEyeDistance(), 
		faceHandler, &detectedFaces, faceTemp);
	
	if (errorCode == IFACE_OK) {
		
		if (detectedFaces != EMPTY_FACE)
		{
			for (int i = 0; i < detectedFaces; i++) {
				float rightEyeX, rightEyeY, leftEyeX, leftEyeY;
				float faceConfidence;
				void* face = faceTemp[i];
				errorCode = IFACE_GetFaceBasicInfo(face, faceHandler,
					&rightEyeX, &rightEyeY, &leftEyeX, &leftEyeY, &faceConfidence);
				error->CheckError(errorCode, error->medium);
				cout << "CONFIDENCE: " << faceConfidence << endl;
				if (faceConfidence > configuration->GetPrecision())
				{
					srand((unsigned)time(NULL));
					int randomName = rand() % 100 + 1;
					string pathImage = nameDirectory + "/" + to_string(randomName) + ".png";
					cout << "GREATER OR EQUAL ACCURACY .." << configuration->GetPrecision() << endl;
					std::thread(&FaceModel::FaceCropImage, this,
						face, pathImage).detach();
					CreateTemplate(face, pathImage, client);
				}
			}
			
		}
	}
	else {
		error->CheckError(errorCode, error->medium);
	}
	for (int i = 0; i < maxFaces; i++) { 
		errorCode = IFACE_ReleaseEntity(faceTemp[i]);
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);

	return detectedFaces;
}