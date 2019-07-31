#ifndef FaceModel_h
#define FaceModel_h

#define BATCH_SIZE	5
#define BATCH_TOTAL_SIZE 30
#define EMPTY_FACE	0

//#include "iface.h"
#include "ConfigurationIFace.h"
#include "ErrorFaceLib.h"
#include "Molded.h"


class FaceModel
{
public:
	FaceModel();
	~FaceModel();
	void SetNameFileImage(string name) {
		nameFileImage = name;
	}

	void SetNameDirectory(string name) {
		nameDirectory = name;
	}

	void Terminate();
	int ModelByBatch(std::vector<std::vector<unsigned char>> bufferOfImagesBatch);
	int ModelOneToOne(vector<unsigned char> buffer);
	
	ConfigurationIFace* configuration = new ConfigurationIFace();
	Rx::subject<Molded*> templateImage;
	Rx::observable<Molded*> observableTemplate = templateImage.get_observable();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	
private:
	const string nameFileCropImage = "imgCrop.png";
	ErrorFaceLib* error = new ErrorFaceLib();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	std::vector<string> pathCropImages;
	string nameFileImage;
	string nameDirectory;
	Rx::subscriber<Molded*> templateOut = templateImage.get_subscriber();
	void FaceCropImage(void* face, string pathImage);	
	int DetectByBatch(void* facesDetected[BATCH_SIZE], 
		std::vector<std::vector<unsigned char>> bufferOfImagesBatch);
	void GetBatchModels(int countFacesDetected, void* facesDetected[BATCH_SIZE]);
	void CreateTemplate(void* face, string pathImageCrop);
	int GetOneModel(unsigned char* rawImage, int width, int height);
	void ObserverError();

};


#endif // !FaceModel_h

