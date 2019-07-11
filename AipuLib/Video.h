#ifndef Video_h
#define Video_h

#include "ErrorAipuLib.h"
#include "ConfigurationVideo.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

class Video
{
public:
	Video();
	~Video();
	void RunVideo();
	vector<uchar> WriteImageOnBuffer(Mat frame);
	void WriteImageOnDisk(Mat frame);
	void WriteBatchOfImagesOnDisk(Mat frame);
	void WriteBatchOfImagesOnBuffer(Mat frame);
	void SetNameDirectory(string name) {
		nameDirectory = name;
		manageFile->SetNameDirectory(name);
		manageFile->CreateDirectory();
	}

	void ResetCountProcessedImages() {
		processedImages = 0;
	}
	
	int GetProcessedImages() {
		//return processedImages;
		return (int)bufferOfImagesBatch.size();
	}

	void StopCapture() {
		flagCapture = 0;
	}
	void SetLapseFrame(int lapse) {
		lapseFrameToFrame = lapse;
	}

	void SetIndexImage(int index) {
		indexExternImage = index;
	}

	void ClearBufferImagesBatch() {
		bufferOfImagesBatch.clear();
	}

	std::vector<std::vector<uchar>> GetBufferOfImages() {
		return bufferOfImagesBatch;
	}

	ConfigurationVideo* configuration = new ConfigurationVideo();
	Rx::subject<Mat> frameImage;
	Rx::observable<Mat> observableImage = frameImage.get_observable();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
private:
	const int quatityImagesSaved = 5;
	ErrorAipuLib* error = new ErrorAipuLib();
	File* manageFile = new File();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<Mat> frameout = frameImage.get_subscriber();
	void CheckFramesBySecond(VideoCapture videoCapture);
	int ChangeIndexImage(int index);
	int batchTotalSize = BATCH_TOTAL_SIZE;
	int processedImages = 0;
	string nameDirectory;
	void ObserverError();
	int flagCapture = 1;
	int lapseFrameToFrame = 5;
	int indexExternImage = 0;
	std::vector<std::vector<uchar>> bufferOfImagesBatch;
};

#endif // !Video_h

