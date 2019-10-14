// AipuLib.cpp : Defines the exported functions for the DLL application.
//

#include "AipuLib.h"
#include "Management.h"
#include "Innovatrics.h"


Management* management;
Innovatrics* innovatrics;
string userJson;
string messageError;
float cropRect[20];

AipuLib::AipuLib()
{	
	innovatrics = new Innovatrics();
	userJson = " ";
	messageError = "There are no errors";
}

AipuLib::~AipuLib()
{
}

void AipuLib::ObserverEvent() {
	

	auto observerErrorManagement = management->observableError.map([](Either* either) {
		return either;
	});


	auto subscriptionErrorManagement = observerErrorManagement.subscribe([this](Either* either) {
		messageError = to_string(either->GetCode()) + ": " + either->GetLabel();

	});

	auto observerDatabase = management->observableUserJSON.map([](string jsonUser) {
		return jsonUser;
	});

	auto subscriptionDatabase = observerDatabase.subscribe([this](string jsonUser) {
		
		userJson = jsonUser;	
		//cout << "USER AIPU: " << userJson << endl;
	});

	auto CoordinatesObservable = management->observableCoordinates.map([](float coordinates[]) {
		return coordinates;
	});

	auto subscriptionCoordinates = CoordinatesObservable.subscribe([this](float coordinates[]) {
		for (int i = 0; i < 20; i++)
		{
			cropRect[i] = coordinates[i];
		}
	/*	cropRect[0] = coordinates[0];
		cropRect[1] = coordinates[1];
		cropRect[2] = coordinates[2];
		cropRect[3] = coordinates[3];
		cropRect[4] = coordinates[4];
		cropRect[5] = coordinates[5];
		cropRect[6] = coordinates[6];
		cropRect[7] = coordinates[7];		
		cropRect[8] = coordinates[8];
		cropRect[9] = coordinates[9];
		cropRect[10] = coordinates[10];
		cropRect[11] = coordinates[11];
		cropRect[12] = coordinates[12];
		cropRect[13] = coordinates[13];
		cropRect[14] = coordinates[14];
		cropRect[15] = coordinates[15];
		cropRect[16] = coordinates[16];
		cropRect[17] = coordinates[17];
		cropRect[18] = coordinates[18];
		cropRect[19] = coordinates[19];
		*/
		/*std::copy(std::begin(cropRect), std::end(cropRect), 
			std::ostream_iterator<float>(std::cout, " "));*/
	});
}

void AipuLib::InitLibrary() {
	auto observerErrorInnovatrics = innovatrics->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionErrorInnovatrics = observerErrorInnovatrics.subscribe([this](Either* either) {
		messageError = to_string(either->GetCode()) + ": " + either->GetLabel();
		
	});
	
	innovatrics->SetParamsLibrary();
	innovatrics->InitLibrary();
	management = new Management();
	ObserverEvent();
}

void AipuLib::LoadConfiguration(string nameFile) {
	management->LoadConfiguration(nameFile);
}

void AipuLib::SetIsRegister(bool option) {
	management->SetIsRegister(option);
}

string AipuLib::GetUserJSON() {
	//cout << "PROPERTY AIPU: " << userJson << endl;
	return userJson;
}

string AipuLib::GetMessageError() {
	
	return messageError;
}

float* AipuLib::GetCoordinates() {
	return cropRect;
}

void AipuLib::SetWorkMode(int mode) {
	management->SetWorkMode(mode);
}

int AipuLib::GetWorkMode() {
	return management->GetWorkMode();
}

void AipuLib::RecognitionFace(unsigned char* image, int rows, int cols, int client) {
	management->RecognitionFace(image, rows, cols, client);
}

void AipuLib::RecognitionFastFace(unsigned char* image, int rows, int cols) {
	management->RecognitionFastFace(image, rows, cols);
}

void AipuLib::InitTracking(unsigned char* image, int rows, int cols) {
	management->InitTracking(image, rows, cols);
}

void AipuLib::Tracking(unsigned char* image, int rows, int cols) {
	management->Tracking(image, rows, cols);
}

void AipuLib::Terminate(int option) {
	innovatrics->Terminate();

	if (option == 0)
	{
		management->Destroy();
	}
	
}

void AipuLib::Reset() {
	innovatrics->SetParamsLibrary();
	innovatrics->InitLibrary();
	management->ReloadFaceModel();
}

void AipuLib::SetSequenceFps(int fps) {
	management->SetSequenceFps(fps);
}

void AipuLib::ResfreshBetweenFrame(int refresh) {
	management->ResfreshBetweenFrame(refresh);
}

void AipuLib::TerminateTracking() {
	management->TerminateTracking();
}

bool AipuLib::GetStateProccessRecognition() {
	return management->GetStateProccessRecognition();
}

void AipuLib::ResetIdUser() {
	management->ResetIdUser();
}

void AipuLib::SetConfigurationDatabase() {
	management->SetConfigurationDatabase();
}

void AipuLib::ResetLowScore() {
	management->ResetLowScore();
}

int AipuLib::GetCountLowScore() {
	return management->GetCountLowScore();
}

void AipuLib::ResetCountRepeatUser() {
	management->ResetCountRepeatUser();
}

int AipuLib::GetCountRepeatUser() {
	return management->GetCountRepeatUser();
}

void AipuLib::ResetCountNotDetect() {
	management->ResetCountNotDetect();
}

int AipuLib::GetCountNotDetect() {
	return management->GetCountNotDetect();
}