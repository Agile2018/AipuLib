#include "Video.h"

Video::Video()
{
	//ObserverError();
}

Video::~Video()
{
}

//void Video::ObserverError() {
//	auto observerError = error->observableError.map([](Either* either) {
//		return either;
//	});
//
//	auto subscriptionError = observerError.subscribe([this](Either* either) {
//		
//		shootError.on_next(either);
//	});
//
//	auto observerErrorConfiguration = configuration->observableError.map([](Either* either) {
//		return either;
//	});
//
//	auto subscriptionErrorConfiguration = observerErrorConfiguration.subscribe([this](Either* either) {
//		
//		shootError.on_next(either);
//	});
//
//}

vector<uchar> Video::WriteImageOnBuffer(Mat frame) {
	vector<uchar> bufferImage;
	int params[3] = { 0 };
	params[0] = IMWRITE_JPEG_QUALITY;
	params[1] = 100;
	bool code = cv::imencode(".jpg", frame,
		bufferImage, std::vector<int>(params, params + 2));	
	return bufferImage;

}


void Video::WriteBatchOfImagesOnBuffer(Mat frame) //Mat frame 
{

	vector<uchar> bufferImage = WriteImageOnBuffer(frame);	
	bufferOfImagesBatch.push_back(bufferImage);

}

