#include "Video.h"

Video::Video()
{
	ObserverError();
}

Video::~Video()
{
}

void Video::ObserverError() {
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


void Video::CheckFramesBySecond(VideoCapture videoCapture) {
	if (configuration->GetFPS().empty()) {
		int fps = (int)(videoCapture.get(CAP_PROP_FPS));
		if (fps < 0) {
			videoCapture.set(CAP_PROP_FPS, FPS);	
			configuration->SetFPSIntToString(FPS);
		}
	}

}

void Video::RunVideo() {
	Mat frame;	
	string pathImageTemp = nameDirectory + "/"; 
	int indexImage = 0;
	int indexBefore = 0;
	flagCapture = 1;

	while (flagCapture == 1) {
		
		if (indexBefore != indexExternImage)
		{
			int indexTemp = indexExternImage;
			indexBefore = indexExternImage;
			indexTemp += 1;
			if (indexTemp == 5)
			{
				indexTemp = 0;
			}
			if (indexTemp == indexImage)
			{
				indexImage = ChangeIndexImage(indexImage);
			}

			string name = "temp" + to_string(indexImage) + ".png";
			string fullName = pathImageTemp + name;	


			frame = imread(fullName, IMREAD_COLOR);
			cout << "INDEX VIDEO: " << indexImage << endl;

			if (frame.data != NULL)
			{				
				frameout.on_next(frame);
			}
				
			indexImage += 1;
			if (indexImage == quatityImagesSaved) {
				indexImage = 0;
			}
			frame.release();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(lapseFrameToFrame));
												
	}
}

int Video::ChangeIndexImage(int index) {
	int indexTemp = index;
	indexTemp += 2;
	switch (indexTemp)
	{
	case 5:
		indexTemp = 0;
		break;
	case 6:
		indexTemp = 1;
		break;
	default:
		break;
	}
	return indexTemp;
}


vector<uchar> Video::WriteImageOnBuffer(Mat frame) {
	vector<uchar> bufferImage;
	int params[3] = { 0 };
	params[0] = IMWRITE_JPEG_QUALITY;
	params[1] = 100;
	bool code = cv::imencode(".jpg", frame,
		bufferImage, std::vector<int>(params, params + 2));
	return bufferImage;

}

void Video::WriteImageOnDisk(Mat frame) {
	string path = nameDirectory + "/" +
		configuration->GetImageTemp();
	imwrite(path, frame);

}
void Video::WriteBatchOfImagesOnDisk(Mat frame) {
	processedImages++;
	
	string fileTemp = nameDirectory + "/" +
		to_string(processedImages) + configuration->GetImageTemp();
	imwrite(fileTemp, frame);

	if (processedImages == batchTotalSize)
	{
		processedImages = 0;
	}

}

void Video::WriteBatchOfImagesOnBuffer(Mat frame) {
	

	vector<uchar> bufferImage = WriteImageOnBuffer(frame);

	bufferOfImagesBatch.push_back(bufferImage);


}

