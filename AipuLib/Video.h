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
	void WriteImageOnDisk(Mat frame);
	void WriteBatchOfImagesOnDisk(Mat frame);
	void SetNameDirectory(string name) {
		nameDirectory = name;
		manageFile->SetNameDirectory(name);
		manageFile->CreateDirectory();
	}

	void ResetCountProcessedImages() {
		processedImages = 0;
	}
	
	int GetProcessedImages() {
		return processedImages;
	}

	
	ConfigurationVideo* configuration = new ConfigurationVideo();
	Rx::subject<Mat> frameImage;
	Rx::observable<Mat> observableImage = frameImage.get_observable();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
private:
	
	ErrorAipuLib* error = new ErrorAipuLib();
	File* manageFile = new File();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<Mat> frameout = frameImage.get_subscriber();
	void CheckFramesBySecond(VideoCapture videoCapture);
	int batchTotalSize = BATCH_TOTAL_SIZE;
	int processedImages = 0;
	string nameDirectory;
	void ObserverError();

};

#endif // !Video_h

