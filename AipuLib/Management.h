#ifndef Management_h
#define Management_h

#include "FaceModel.h"
#include "FlowTrend.h"
#include "Train.h"
#include "Movement.h"
#include "Video.h"
#include "ConfigurationFile.h"
#include "Identify.h"
#include <ctime>
#include "Database.h"


#define SINGLE			1
#define OPTION_BATCH	2
#define MOVEMENT		3
#define ONE_MINUTE		1
#define SIXTY_SECONDS	60
#define SIXTY_MINUTES	60
#define SIZE_BUFFER		5

const string DIRECTORY_CONFIGURATION = "configuration";

using namespace std;

class Management
{
public:
	Management();
	~Management();

	void LoadConfiguration(string nameFile) {
		configurationFile->SetNameFileConfiguration(nameFile);
		configurationFile->ParseJSONToObject();
		//SetNameFileConfigurationVideo(configurationFile->GetNameFileConfigurationVideo());
		SetNameFileConfigurationFace(configurationFile->GetNameFileConfigurationFaceModel());
		video->SetNameDirectory(configurationFile->GetNameDirectoryTempWork());
		faceModel->SetNameDirectory(configurationFile->GetNameDirectoryTempWork());
		train->CreateTrainingDirectory(configurationFile->GetNameDirectoryTraining());
		flowTrend->SetDirectory(configurationFile->GetNameDirectoryTraining());
		SetNameFileConfigurationDatabase(configurationFile->GetNameFileConfigurationDatabase());		
		database->Configure();
	}
	void SetNameDatabase(string name) {
		database->configuration->SetNameDatabase(name);
	}
	void SetConnectStringDatabase(string connect) {
		database->configuration->SetConnectString(connect);
	}

	void ResetIdUser() {
		database->ResetIdUser();
	}

	void SaveConfigurationDatabase(string nameFile) {
		database->configuration->SetNameFileConfiguration(nameFile);
		database->configuration->ParseMapToJSON();
		configurationFile->SetNameFileConfigurationDatabase(nameFile);
	}

	void SetNameDirectoryWorkTemporal(string name) {
		configurationFile->SetNameDirectoryTempWork(name);

	}

	void SetNameDirectoryTraining(string name) {		
		configurationFile->SetNameDirectoryTraining(name);
		
	}

	//void SetPathVideo(string valuePath) {
	//	video->configuration->SetPath(valuePath);
	//}
	//void SetUserVideo(string valueUser) {
	//	video->configuration->SetUser(valueUser);
	//}
	//void SetPasswordVideo(string valuePassword) {
	//	video->configuration->SetPassword(valuePassword);
	//}
	//void SetResolutionVideo(string valueResolution) {
	//	video->configuration->SetResolution(valueResolution);
	//}
	//void SetFPSVideo(string valueFps) {
	//	video->configuration->SetFPS(valueFps);
	//}
	//void SetImageTempVideo(string name) {
	//	video->configuration->SetImageTemp(name);
	//}

	/*void SaveConfigurationVideo(string nameFile) {
		video->configuration->SetNameFileConfiguration(nameFile);
		video->configuration->ParseMapToJSON();		
		configurationFile->SetNameFileConfigurationVideo(nameFile);
	}*/

	void SetMinEyeDistance(int minDistance) {
		faceModel->configuration->SetMinEyeDistance(minDistance);
	}
	void SetMaxEyeDistance(int maxDistance) {
		faceModel->configuration->SetMaxEyeDistance(maxDistance);
	}

	void SetMaxDetect(int maxFacesDetect) {
		faceModel->configuration->SetMaxDetect(maxFacesDetect);
	}

	void SetPrecision(int precision) {
		faceModel->configuration->SetPrecision(precision);
	}

	void SaveConfigurationFaceModel(string nameFile) {
		faceModel->configuration->SetNameFileConfiguration(nameFile);
		faceModel->configuration->ParseMapToJSON();
		configurationFile->SetNameFileConfigurationFaceModel(nameFile);
	}

	void SaveConfigurationFile(string nameFile) {
		configurationFile->SetNameFileConfiguration(nameFile);
		configurationFile->ParseMapToJSON();
	}

	/*string GetStringJSONVideo() {
		return video->configuration->GetStringJSON();
	}*/

	string GetStringJSONFaceModel() {
		return faceModel->configuration->GetStringJSON();
	}
	string GetStringJSONFiles() {
		return configurationFile->GetStringJSON();
	}

	
	void SetIsRegister(bool option) {
		identify->SetIsRegister(option);
	}	

	void SetWorkMode(int mode) {
		workMode = mode;
	}

	int GetWorkMode() {
		return workMode;
	}

	bool GetStateProccessRecognition() {
		return flagNextFrame;
	}

	void RecognitionFace(unsigned char* image, 
		int rows, int cols, int client);

	void RecognitionFastFace(unsigned char* image,
		int rows, int cols);

	void InitTracking(unsigned char* image,
		int rows, int cols);

	void Tracking(unsigned char* image,
		int rows, int cols);

	void SetSequenceFps(int fps) {
		faceModel->SetSequenceFps(fps);
	}

	void ResfreshBetweenFrame(int refresh) {
		faceModel->ResfreshBetweenFrame(refresh);
	}

	void TerminateTracking() {
		faceModel->TerminateTracking();
	}

	void Destroy() {
		delete configurationFile;
		delete faceModel;
		delete video;
		delete flowTrend;
		delete train;
		delete movement;
		delete identify;
		delete database;
	}

	void ReloadFaceModel() {
		faceModel->configuration->ParseJSONToObject();		
	}

	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();

	Rx::subject<string> userDetected;
	Rx::observable<string> observableUserJSON = userDetected.get_observable();

	Rx::subject<float*> subjectCoordinates;
	Rx::observable<float*> observableCoordinates = subjectCoordinates.get_observable();

private:	
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();	
	Rx::subscriber<string> shootUserJSON = userDetected.get_subscriber();
	Rx::subscriber<float*> shootCoordinates = subjectCoordinates.get_subscriber();
	ConfigurationFile* configurationFile = new ConfigurationFile();
	FaceModel* faceModel = new FaceModel();
	Video* video = new Video();
	FlowTrend* flowTrend = new FlowTrend();
	Train* train = new Train();
	Movement* movement = new Movement();
	Identify* identify = new Identify();
	Database* database = new Database();
	std::chrono::steady_clock::time_point startCountTime = std::chrono::steady_clock::now();
	int countImagesDetected = 0;
	int workMode = SINGLE;
	bool flagNextFrame = false;
	bool flagFastNextFrame = false;
	double timeDurationSingleDetection = 0;
	void SetDirectoryConfiguration();
	void ObserverError();
	void SaveDataTraining(int quantityDetected, int day, int hour, int minute);
	void VerifyTrainingLapse();
	int SetStateFlow(int minute);
	void GetModelOneToOne(Mat image, int client);
	void GetModelsByBatch(Mat image, int client);
	Mat ByteToMat(unsigned char* image, int rows, int cols);	
	void ObserverTemplateImage();
	void ObserverIdentifyFace();
	void ObserverDatabase();
	void ObserverCoordinatesFace();
	/*void SetNameFileConfigurationVideo(string name) {
		video->configuration->SetNameFileConfiguration(name);
		video->configuration->ParseJSONToObject();
		faceModel->SetNameFileImage(video->configuration->GetImageTemp());

	}*/

	void SetNameFileConfigurationFace(string name) {
		faceModel->configuration->SetNameFileConfiguration(name);
		faceModel->configuration->ParseJSONToObject();

	}

	void SetNameFileConfigurationDatabase(string name) {
		database->configuration->SetNameFileConfiguration(name);
		database->configuration->ParseJSONToObject();
	}

	void ProcessImage(Mat image, int client);
	void ProcessFastImage(Mat image);
	void ProcessInitTracking(Mat image, int rows, int cols);
	void ProcessTracking(Mat image, int rows, int cols);
};

#endif // !Management_h

