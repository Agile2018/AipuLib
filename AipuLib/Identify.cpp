#include "Identify.h"

Identify::Identify()
{
	ObserverError();
	LoadConnection();
}

Identify::~Identify()
{
	IEngine_TerminateModule();
}

void Identify::LoadConnection() {
	int errorCode;
	std::string str = "iengine.db";
	const char *cstr = str.c_str();

	errorCode = IEngine_InitModule();
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_SetParameter(CFG_IDENTIFICATION_SPEED, 4);
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_SetParameter(CFG_MAX_ROTATION, 90);
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_Connect(cstr);
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_ClearDatabase();
	error->CheckError(errorCode, error->medium);

}

void Identify::EnrollUser(Molded* modelImage) {
	int errorCode, userID, score;
	IENGINE_USER user = IEngine_InitUser();
	const unsigned char* templateData = reinterpret_cast<const unsigned char*>(modelImage->GetMoldImage());
	
	errorCode = IEngine_AddFaceTemplate(user, templateData, modelImage->GetMoldSize());
	if (errorCode == IENGINE_E_NOERROR) {
		User* userForDatabase = new User();

		errorCode = IEngine_Connect("iengine.db");
		error->CheckError(errorCode, error->medium);		

		errorCode = IEngine_FindUser(user, &userID, &score);		
		if (userID == 0)
		{

			errorCode = IEngine_RegisterUser(user, &userID);			
			error->CheckError(errorCode, error->medium);
			if (errorCode == IENGINE_E_NOERROR) {
				userForDatabase->SetIsNew(true);
			}
		}
		if (errorCode == IENGINE_E_NOERROR) {
			userForDatabase->SetUserIdIFace(userID);
			userForDatabase->SetPathImageTemp(modelImage->GetPathImage());
			shootUser.on_next(userForDatabase);
		}
	}
	else {
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IEngine_FreeUser(user);
	error->CheckError(errorCode, error->medium);
}

void Identify::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {		
		shootError.on_next(either);
	});
}
