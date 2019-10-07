#ifndef FaceModel_h
#define FaceModel_h

#define BATCH_SIZE	5
#define BATCH_TOTAL_SIZE 30
#define EMPTY_FACE	0
#define TRACKED_OBJECTS	5
#define SIZE_COORDINATES 20
#define NUMBER_COORDINATES_IMAGES  4

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

	void SetSequenceFps(int fps) {
		sequenceFps = fps;
		timeDeltaMs = 1000 / sequenceFps;
	}

	void ResfreshBetweenFrame(int refresh) {
		refreshInterval = refresh;
	}

	void Terminate();
	void TerminateTracking();
	int ModelByBatch(std::vector<std::vector<unsigned char>> bufferOfImagesBatch, 
		std::vector<int> clients);
	int ModelOneToOne(vector<unsigned char> buffer, int client);
	void FastOnlyDetect(vector<unsigned char> buffer);
	void Tracking(vector<unsigned char> buffer, int width, int height);
	void InitTracking(vector<unsigned char> buffer, int width, int height);
	ConfigurationIFace* configuration = new ConfigurationIFace();
	Rx::subject<Molded*> templateImage;
	Rx::observable<Molded*> observableTemplate = templateImage.get_observable();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	Rx::subject<float*> subjectCoordinates;
	Rx::observable<float*> observableCoordinates = subjectCoordinates.get_observable();

private:
	const string nameFileCropImage = "imgCrop.png";
	ErrorFaceLib* error = new ErrorFaceLib();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<float*> shootCoordinates = subjectCoordinates.get_subscriber();
	Rx::subscriber<Molded*> templateOut = templateImage.get_subscriber();
	std::vector<string> pathCropImages;
	std::vector<int> clientsId;
	
	float imageCoordinatesFollowed[SIZE_COORDINATES];

	string nameFileImage;
	string nameDirectory;	
	int sequenceFps = 30;
	int timeDeltaMs = 1000 / sequenceFps;
	int refreshInterval = 2000; // time interval (in ms) between frames where refresh of face detect will be done	
	void* faceHandlerTracking;
	void* objectHandler;
	void* objects[TRACKED_OBJECTS];
	//void* faceTracking;
	//vector<void *> allTrackedObjects;
	int countFrameTracking = 0;
	void FaceCropImage(void* face, string pathImage);	
	int DetectByBatch(void* facesDetected[BATCH_TOTAL_SIZE],
		std::vector<std::vector<unsigned char>> bufferOfImagesBatch, std::vector<int> clients);
	void GetBatchModels(int countFacesDetected, void* facesDetected[BATCH_TOTAL_SIZE]);
	void CreateTemplate(void* face, string pathImageCrop, int client);
	int GetOneModel(unsigned char* rawImage, int width, int height, int client);
	void FastDetect(unsigned char* rawImage, int width, int height);
	void ObserverError();
	void GetFaceCropRectangle(void* face);	
	unsigned char* LoadImage(vector<unsigned char> buffer);
	unsigned char* LoadFileImage(string image, int *width, int *height, int *length);
	void BuildCoordinates(float x, float y, float width, float height, int indexTracked);
	/*int SearchId(int idTracked);
	void DeleteId(int idTracked);
	void SuspendIndex(int idTracked);*/
	void ClearCoordinatesImage(int indexTracked);
};


#endif // !FaceModel_h

