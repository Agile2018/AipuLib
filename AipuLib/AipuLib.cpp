// AipuLib.cpp : Defines the exported functions for the DLL application.
//

#include "AipuLib.h"
#include "Management.h"
#include "Innovatrics.h"


Management* management;
Innovatrics* innovatrics;
string userJson;
string messageError;


AipuLib::AipuLib()
{	
	innovatrics = new Innovatrics();
	userJson = " ";
	messageError = "There are no errors";
}

AipuLib::~AipuLib()
{
}

void AipuLib::ObserverError() {
	

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
		cout << "USER AIPU: " << userJson << endl;
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
	ObserverError();
}

void AipuLib::LoadConfiguration(string nameFile) {
	management->LoadConfiguration(nameFile);
}

void AipuLib::SetIsRegister(bool option) {
	management->SetIsRegister(option);
}

string AipuLib::GetUserJSON() {
	cout << "PROPERTY AIPU: " << userJson << endl;
	return userJson;
}

string AipuLib::GetMessageError() {
	
	return messageError;
}


void AipuLib::SetWorkMode(int mode) {
	management->SetWorkMode(mode);
}

int AipuLib::GetWorkMode() {
	return management->GetWorkMode();
}

void AipuLib::RecognitionFace(unsigned char* image, int rows, int cols) {
	management->RecognitionFace(image, rows, cols);
}