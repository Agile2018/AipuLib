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
#define MULTIPLE_OF_TEN 10

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
		SetNameFileConfigurationVideo(configurationFile->GetNameFileConfigurationVideo());
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

	void SetPathVideo(string valuePath) {
		video->configuration->SetPath(valuePath);
	}
	void SetUserVideo(string valueUser) {
		video->configuration->SetUser(valueUser);
	}
	void SetPasswordVideo(string valuePassword) {
		video->configuration->SetPassword(valuePassword);
	}
	void SetResolutionVideo(string valueResolution) {
		video->configuration->SetResolution(valueResolution);
	}
	void SetFPSVideo(string valueFps) {
		video->configuration->SetFPS(valueFps);
	}
	void SetImageTempVideo(string name) {
		video->configuration->SetImageTemp(name);
	}

	void SaveConfigurationVideo(string nameFile) {
		video->configuration->SetNameFileConfiguration(nameFile);
		video->configuration->ParseMapToJSON();		
		configurationFile->SetNameFileConfigurationVideo(nameFile);
	}

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

	string GetStringJSONVideo() {
		return video->configuration->GetStringJSON();
	}

	string GetStringJSONFaceModel() {
		return faceModel->configuration->GetStringJSON();
	}
	string GetStringJSONFiles() {
		return configurationFile->GetStringJSON();
	}

	void RunVideo();

	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();

	Rx::subject<Mat> frame;
	Rx::observable<Mat> observableFrame = frame.get_observable();

	Rx::subject<string> userDetected;
	Rx::observable<string> observableUserJSON = userDetected.get_observable();


private:	
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<Mat> frameOut = frame.get_subscriber();
	Rx::subscriber<string> shootUserJSON = userDetected.get_subscriber();
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
	double timeDurationSingleDetection = 0;
	void SetDirectoryConfiguration();
	void ObserverError();
	void SaveDataTraining(int quantityDetected, int day, int hour, int minute);
	void VerifyTrainingLapse();
	int SetStateFlow(int minute);
	void GetModelOneToOne(Mat image);
	void GetModelsByBatch(Mat image);
	void ProcessImage(Mat image);
	void ObserverVideo();
	void ObserverTemplateImage();
	void ObserverIdentifyFace();
	void ObserverDatabase();
	void SetNameFileConfigurationVideo(string name) {
		video->configuration->SetNameFileConfiguration(name);
		video->configuration->ParseJSONToObject();
		faceModel->SetNameFileImage(video->configuration->GetImageTemp());

	}

	void SetNameFileConfigurationFace(string name) {
		faceModel->configuration->SetNameFileConfiguration(name);
		faceModel->configuration->ParseJSONToObject();

	}

	void SetNameFileConfigurationDatabase(string name) {
		database->configuration->SetNameFileConfiguration(name);
		database->configuration->ParseJSONToObject();
	}

};

#endif // !Management_h

