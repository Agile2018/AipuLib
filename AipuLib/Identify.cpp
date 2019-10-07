#include "Identify.h"

Identify::Identify()
{
	ObserverError();
	
}

Identify::~Identify()
{
	//IEngine_TerminateModule();
}

void Identify::LoadConnection() {
	int errorCode;
	std::string str = "iengine.db";
	const char *cstr = str.c_str();
	SetParamsIdentify();
	errorCode = connectToDatabase(cstr);
	cout << "connectToDatabase returns " << errorCode << endl;
	error->CheckError(errorCode, error->medium);


	/*errorCode = IEngine_InitModule();
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_SetParameter(CFG_IDENTIFICATION_SPEED, 4);
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_SetParameter(CFG_MAX_ROTATION, 90);
	error->CheckError(errorCode, error->medium);
	errorCode = IEngine_Connect(cstr);
	error->CheckError(errorCode, error->medium);*/


	/*errorCode = IEngine_ClearDatabase();
	error->CheckError(errorCode, error->medium);*/

}

void Identify::SetParamsIdentify() {
	int errorCode;
	//std::string paramsIdentify = configuration->ParseMapToJSONForIdentify();	
	std::string paramsIdentify = format->FormatString("{\"A_IdentificationSpeed\": %d, \"A_MinEyeDist\": %d, \"A_MaxEyeDist\": %d, \"A_FaceDetectionForced\": %d}",
		configuration->GetIdentificationSpeed(), 
		configuration->GetMinEyeDistance(), 
		configuration->GetMaxEyeDistance(), configuration->GetFaceDetectionForced());
	errorCode = setBiometricParameters(paramsIdentify.c_str());
	//cout << "setBiometricParameters params " << paramsIdentify << endl;
	cout << "setBiometricParameters returns " << errorCode << endl;
	error->CheckError(errorCode, error->medium);
	
	// "{\"A_IdentificationSpeed\": 5, \"A_MinEyeDist\": 35, \"A_MaxEyeDist\": 250, \"A_FaceDetectionForced\": 2}"
}

void Identify::EnrollUser(Molded* modelImage) {
	int errorCode, userID, score;
	//IENGINE_USER user = IEngine_InitUser();
	User* userForDatabase = new User();
	const unsigned char* templateData = reinterpret_cast<const unsigned char*>(modelImage->GetMoldImage());	

	errorCode = identify(templateData, modelImage->GetMoldSize(), &userID, &score);
	if (userID == 0 && isRegister)
	{

		errorCode = addUserToDatabase(templateData, modelImage->GetMoldSize(), &userID);
		error->CheckError(errorCode, error->medium);
		if (errorCode == IENGINE_E_NOERROR) {
			userForDatabase->SetIsNew(true);
		}
	}
	if (errorCode == IENGINE_E_NOERROR && userID != 0) {
		userForDatabase->SetUserIdIFace(userID);
		userForDatabase->SetPathImageTemp(modelImage->GetPathImage());
		userForDatabase->SetClient(modelImage->GetIdClient());
		shootUser.on_next(userForDatabase);
	}


	/*errorCode = IEngine_AddFaceTemplate(user, templateData, modelImage->GetMoldSize());
	if (errorCode == IENGINE_E_NOERROR) {
		User* userForDatabase = new User();

		errorCode = IEngine_Connect("iengine.db");
		error->CheckError(errorCode, error->medium);		

		errorCode = IEngine_FindUser(user, &userID, &score);		
		if (userID == 0 && isRegister)
		{

			errorCode = IEngine_RegisterUser(user, &userID);			
			error->CheckError(errorCode, error->medium);
			if (errorCode == IENGINE_E_NOERROR) {
				userForDatabase->SetIsNew(true);
			}
		}
		if (errorCode == IENGINE_E_NOERROR && userID != 0) {
			userForDatabase->SetUserIdIFace(userID);
			userForDatabase->SetPathImageTemp(modelImage->GetPathImage());
			userForDatabase->SetClient(modelImage->GetIdClient());
			shootUser.on_next(userForDatabase);
		}
	}
	else {
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IEngine_FreeUser(user);
	error->CheckError(errorCode, error->medium);*/
}

void Identify::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {		
		shootError.on_next(either);
	});
}
