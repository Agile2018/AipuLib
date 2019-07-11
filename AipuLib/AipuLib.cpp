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

void AipuLib::RunVideo() {
	management->RunVideo();
}

string AipuLib::GetUserJSON() {
	cout << "PROPERTY AIPU: " << userJson << endl;
	return userJson;
}

string AipuLib::GetMessageError() {
	
	return messageError;
}

void AipuLib::StopVideo() {
	management->StopCapture();
}

void AipuLib::SetLapseFrameToFrame(int lapse) {
	management->SetLapseFrame(lapse);
}

void AipuLib::SetIndexImage(int index) {
	management->SetIndexImage(index);
}

