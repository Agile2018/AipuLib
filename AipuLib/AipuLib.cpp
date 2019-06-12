// AipuLib.cpp : Defines the exported functions for the DLL application.
//

#include "AipuLib.h"

AipuLib::AipuLib()
{
	
}

AipuLib::~AipuLib()
{
}

void AipuLib::ObserverError() {
	

	auto observerErrorManagement = management->observableError.map([](Either* either) {
		return either;
	});


	auto subscriptionErrorManagement = observerErrorManagement.subscribe([this](Either* either) {
		string messageError = to_string(either->GetCode()) + ": " + either->GetLabel();
		shootError.on_next(messageError);

	});

	auto observerFrame = management->observableFrame.map([](Mat img) {
		return img;
	});

	auto subscriptionFrame = observerFrame.subscribe([this](Mat img) {
		frameOut.on_next(img);
	});

	auto observerDatabase = management->observableUserJSON.map([](string jsonUser) {
		return jsonUser;
	});

	auto subscriptionDatabase = observerDatabase.subscribe([this](string jsonUser) {
		shootUserJSON.on_next(jsonUser);

	});
}

void AipuLib::InitLibrary() {
	auto observerErrorInnovatrics = innovatrics->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionErrorInnovatrics = observerErrorInnovatrics.subscribe([this](Either* either) {
		string messageError = to_string(either->GetCode()) + ": " + either->GetLabel();
		shootError.on_next(messageError);
	});

	innovatrics->SetParamsLibrary();
	innovatrics->InitLibrary();
	management = new Management();
	ObserverError();
}

void AipuLib::LoadConfiguration(string nameFile) {
	management->LoadConfiguration(nameFile);
}

void AipuLib::RunVideo() {
	management->RunVideo();
}
