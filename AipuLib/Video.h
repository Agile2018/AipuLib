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
	
	vector<uchar> WriteImageOnBuffer(Mat frame);
	vector<uchar> WriteImagePngOnBuffer(Mat frame);
	
	void WriteBatchOfImagesOnBuffer(Mat frame, int client);
	void SetNameDirectory(string name) {
		nameDirectory = name;
		manageFile->SetNameDirectory(name);
		manageFile->CreateDirectory();
	}

	int GetProcessedImages() {
		
		return (int)bufferOfImagesBatch.size();
	}	

	void ClearBufferImagesBatch() {
		bufferOfImagesBatch.clear();
	}

	std::vector<std::vector<uchar>> GetBufferOfImages() {
		return bufferOfImagesBatch;
	}

	void ClearBufferClientsBatch() {
		clients.clear();
	}

	std::vector<int> GetBufferOfClients() {
		return clients;
	}

	/*ConfigurationVideo* configuration = new ConfigurationVideo();
	
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();*/
private:
	
	//ErrorAipuLib* error = new ErrorAipuLib();
	File* manageFile = new File();
	//Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	
	string nameDirectory;
	//void ObserverError();
	std::vector<int> clients;
	std::vector<std::vector<uchar>> bufferOfImagesBatch;
};

#endif // !Video_h

