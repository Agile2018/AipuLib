#include "FaceModel.h"

FaceModel::FaceModel()
{
	ObserverError();
}

FaceModel::~FaceModel()
{
	Terminate();
}

void FaceModel::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {
		
		shootError.on_next(either);
	});

	auto observerErrorConfiguration = configuration->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionErrorConfiguration = observerErrorConfiguration.subscribe([this](Either* either) {
		
		shootError.on_next(either);
	});

}

void FaceModel::Terminate() {
	int errorCode;	
	
	errorCode = IFACE_Terminate();
	error->CheckError(errorCode, error->less);

}

void FaceModel::TerminateTracking() {
	int errorCode;
	for (int i = 0; i < TRACKED_OBJECTS; i++)
	{
		try
		{
			if (objects[i] != NULL)
			{
				errorCode = IFACE_ReleaseEntity(objects[i]);
				error->CheckError(errorCode, error->less);
			}
			
		}
		catch (const std::exception& e)
		{			
			cout << e.what() << endl;
		}		
		
	}
	/*for (size_t i = 0; i < allTrackedObjects.size(); i++)
	{
		errorCode = IFACE_ReleaseEntity(allTrackedObjects[i]);
		error->CheckError(errorCode, error->less);
	}
	allTrackedObjects.clear();*/
	/*errorCode = IFACE_ReleaseEntity(faceTracking);
	error->CheckError(errorCode, error->medium);*/

	errorCode = IFACE_ReleaseEntity(objectHandler);
	error->CheckError(errorCode, error->less);

	errorCode = IFACE_ReleaseEntity(faceHandlerTracking);
	error->CheckError(errorCode, error->less);
	//coordinatesIdUser.clear();
}

int FaceModel::DetectByBatch(void* facesDetected[BATCH_TOTAL_SIZE],
	std::vector<std::vector<unsigned char>> bufferOfImagesBatch, 
	std::vector<int> clients) {
	int errorCode, countFacesDetected = 0;
	void* faceHandler;

	srand((unsigned)time(NULL));
	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);

	if (configuration->GetModeDetect() == 1)
	{
		errorCode = IFACE_SetParam(faceHandler,
			IFACE_PARAMETER_FACEDET_SPEED_ACCURACY_MODE,
			IFACE_FACEDET_SPEED_ACCURACY_MODE_BALANCED);
		error->CheckError(errorCode, error->medium);
	}
	if (configuration->GetModeDetect() == 2)
	{
		errorCode = IFACE_SetParam(faceHandler,
			IFACE_PARAMETER_FACEDET_SPEED_ACCURACY_MODE,
			IFACE_FACEDET_SPEED_ACCURACY_MODE_FAST);
		error->CheckError(errorCode, error->medium);
	}
	
	
	for (int i = 0; i < BATCH_TOTAL_SIZE; i++)
	{

		errorCode = IFACE_CreateFace(&(facesDetected[i]));
		error->CheckError(errorCode, error->medium);
	}

	for (int i = 0; i < BATCH_SIZE; i++)
	{
		int lenght, widthFace, heightFace;

		vector<unsigned char> buffer = bufferOfImagesBatch[i];
		const char* imgData = reinterpret_cast<const char*> (&buffer[0]);

		if (imgData != NULL) {
			errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &widthFace,
				&heightFace, &lenght, NULL);
			if (errorCode == IFACE_OK)
			{
				int maxFaces = configuration->GetMaxDetect();
				unsigned char* rawImage = new unsigned char[lenght];
				errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &widthFace,
					&heightFace, &lenght, rawImage);

				error->CheckError(errorCode, error->medium);
				void** faceTemp = new void*[maxFaces];

				for (int j = 0; j < maxFaces; j++) {
					errorCode = IFACE_CreateFace(&(faceTemp[j]));
					error->CheckError(errorCode, error->medium);
				}

				int detectedFaces = configuration->GetMaxDetect();
		
				errorCode = IFACE_DetectFaces(rawImage, widthFace, heightFace,
					configuration->GetMinEyeDistance(), configuration->GetMaxEyeDistance(),
					faceHandler, &detectedFaces, faceTemp);
				delete rawImage;
				error->CheckError(errorCode, error->medium);

				if (detectedFaces != EMPTY_FACE)
				{
					for (int j = 0; j < detectedFaces; j++) {
						float rightEyeX, rightEyeY, leftEyeX, leftEyeY;
						float faceConfidence;
						void* face = faceTemp[j];
						errorCode = IFACE_GetFaceBasicInfo(face, faceHandler,
							&rightEyeX, &rightEyeY, &leftEyeX, &leftEyeY, &faceConfidence);
						error->CheckError(errorCode, error->medium);
						cout << "CONFIDENCE: " << faceConfidence << endl;
						if (faceConfidence > configuration->GetPrecision() && 
							countFacesDetected < BATCH_TOTAL_SIZE)
						{

							errorCode = IFACE_CloneEntity(face, facesDetected[countFacesDetected]);
							error->CheckError(errorCode, error->medium);
							int randomName = rand() % 100 + 1;
							string pathImage = nameDirectory + "/" + to_string(randomName) + ".png";
							cout << "GREATER OR EQUAL ACCURACY .." << configuration->GetPrecision() << endl;
							std::thread(&FaceModel::FaceCropImage, this,
								facesDetected[countFacesDetected], pathImage).detach();
							pathCropImages.push_back(pathImage);
							clientsId.push_back(clients[i]);
							countFacesDetected++;
						}
					}

				}

				for (int j = 0; j < maxFaces; j++) {
					errorCode = IFACE_ReleaseEntity(faceTemp[j]);
					error->CheckError(errorCode, error->medium);
				}
				
			}
			else {
				error->CheckError(errorCode, error->medium);
			}

		}


	}
	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);

	return countFacesDetected;
}
void FaceModel::GetFaceCropRectangle(void* face) {
	float cropRect[8];
	int errorCode;
	void* faceHandler;
	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_GetFaceCropRectangle(face, faceHandler, 
		IFACE_FACE_CROP_METHOD_FULL_FRONTAL, cropRect);
	error->CheckError(errorCode, error->medium);
	if (errorCode == IFACE_OK) {
		shootCoordinates.on_next(cropRect);
	}
	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);
}

void FaceModel::FaceCropImage(void* face, string pathImage) {
	int cropWidth, cropHeight, cropLength, errorCode;
	void* faceHandler;

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);


	errorCode = IFACE_GetFaceCropImage(face, faceHandler, IFACE_FACE_CROP_METHOD_FULL_FRONTAL, 
		&cropWidth, &cropHeight, &cropLength, NULL);
	error->CheckError(errorCode, error->medium);
	unsigned char* cropImageData = new unsigned char[cropLength];
	errorCode = IFACE_GetFaceCropImage(face, faceHandler, IFACE_FACE_CROP_METHOD_FULL_FRONTAL, 
		&cropWidth, &cropHeight, &cropLength, cropImageData);
	error->CheckError(errorCode, error->medium);
		
	if (errorCode == IFACE_OK) {
		errorCode = IFACE_SaveImage(pathImage.c_str(), cropWidth, cropHeight, 3, cropImageData);
	}
	error->CheckError(errorCode, error->medium);
			
	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);
	delete[] cropImageData;
}

int FaceModel::ModelByBatch(std::vector<std::vector<unsigned char>> bufferOfImagesBatch, 
	std::vector<int> clients) {
	int errorCode;
	void* facesDetected[BATCH_TOTAL_SIZE];
	int countFacesDetected = DetectByBatch(facesDetected, bufferOfImagesBatch, clients);
	if (countFacesDetected != 0)
	{
		GetBatchModels(countFacesDetected, facesDetected);
	}	

	for (int i = 0; i < BATCH_TOTAL_SIZE; i++)
	{
		errorCode = IFACE_ReleaseEntity(facesDetected[i]);
		error->CheckError(errorCode, error->medium);
	}
	return countFacesDetected;
}

void FaceModel::GetBatchModels(int countFacesDetected, void* facesDetected[BATCH_TOTAL_SIZE]) {
	int errorCode;
	int templateBatchDataSize;
	void** cloneFacesDetected = new void*[countFacesDetected];
	char** batchTemplates = new char*[countFacesDetected];
	void* faceHandler;

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);

	for (int i = 0; i < countFacesDetected; i++)
	{
		errorCode = IFACE_CreateFace(&(cloneFacesDetected[i]));
		error->CheckError(errorCode, error->medium);
	}

	for (int i = 0; i < countFacesDetected; i++)
	{
		errorCode = IFACE_CloneEntity(facesDetected[i], cloneFacesDetected[i]);
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_CreateTemplateBatch(faceHandler, countFacesDetected, cloneFacesDetected,
		&templateBatchDataSize, NULL);
	error->CheckError(errorCode, error->medium);

	for (int i = 0; i < countFacesDetected; ++i)
	{

		batchTemplates[i] = new char[templateBatchDataSize];
	}

	errorCode = IFACE_CreateTemplateBatch(faceHandler, countFacesDetected, cloneFacesDetected,
		&templateBatchDataSize, batchTemplates);

	error->CheckError(errorCode, error->medium);
	
	for (int i = 0; i < countFacesDetected; i++)
	{
		//string pathImage = nameDirectory + "/" + to_string(countFacesDetected) + nameFileCropImage;
		Molded* model = new Molded();
		model->SetMoldImage(batchTemplates[i]);
		model->SetMoldSize(templateBatchDataSize);
		model->SetPathImage(pathCropImages[i]);
		model->SetIdClient(clientsId[i]);
		templateOut.on_next(model);
		
	}	

	for (int i = 0; i < countFacesDetected; ++i)
	{
		delete batchTemplates[i];
	}
	for (int i = 0; i < countFacesDetected; i++)
	{
		errorCode = IFACE_ReleaseEntity(cloneFacesDetected[i]);
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);
	pathCropImages.clear();
	clientsId.clear();
}

unsigned char* FaceModel::LoadImage(vector<unsigned char> buffer) {
	int lenght, width, height, errorCode;
	const char* imgData = reinterpret_cast<const char*> (&buffer[0]);
	if (imgData == NULL) {
		return NULL;
	}
	
	errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &width,
		&height, &lenght, NULL);
	if (errorCode != IFACE_OK) {
		error->CheckError(errorCode, error->medium);
		return NULL;
	}
	unsigned char* rawImage = new unsigned char[lenght];
	errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &width,
		&height, &lenght, rawImage);
	if (errorCode != IFACE_OK) {
		error->CheckError(errorCode, error->medium);
		return NULL;
	}

	return rawImage;
	
}

void FaceModel::FastOnlyDetect(vector<unsigned char> buffer) {
	int lenght, width, height, errorCode;
	const char* imgData = reinterpret_cast<const char*> (&buffer[0]);
	if (imgData != NULL) {
		errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &width,
			&height, &lenght, NULL);
		if (errorCode == IFACE_OK)
		{

			unsigned char* rawImage = new unsigned char[lenght];
			errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &width,
				&height, &lenght, rawImage);
			if (errorCode != IFACE_OK)
			{
				error->CheckError(errorCode, error->medium);
			}
			else
			{
				FastDetect(rawImage, width, height);
			}
			delete rawImage;
		}
		else {
			error->CheckError(errorCode, error->medium);
		}
	}

}

void FaceModel::InitTracking(vector<unsigned char> buffer, int width, int height) {
	int errorCode;		
	
	unsigned char* maskImage = LoadImage(buffer);
	
	errorCode = IFACE_CreateFaceHandler(&faceHandlerTracking);
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_CreateObjectHandler(&objectHandler, faceHandlerTracking);
	error->CheckError(errorCode, error->medium);
	/*errorCode = IFACE_CreateFace(&faceTracking);
	error->CheckError(errorCode, error->medium);*/
	for (int i = 0; i < TRACKED_OBJECTS; i++)
	{
		errorCode = IFACE_CreateObject(&objects[i]);
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_SetParam(objectHandler, 
		IFACE_PARAMETER_TRACK_FACE_DISCOVERY_FREQUENCE_MS, 
		std::to_string(refreshInterval).c_str());
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_SetParam(objectHandler, 
		IFACE_PARAMETER_TRACK_MIN_EYE_DISTANCE, 
		std::to_string(configuration->GetMinEyeDistance()).c_str());
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_SetParam(objectHandler, 
		IFACE_PARAMETER_TRACK_MAX_EYE_DISTANCE, 
		std::to_string(configuration->GetMaxEyeDistance()).c_str());
	error->CheckError(errorCode, error->medium);
	
	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_TRACKING_MODE,
		IFACE_TRACK_TRACKING_MODE_OBJECT_TRACKING);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_SPEED_ACCURACY_MODE,
		IFACE_TRACK_SPEED_ACCURACY_MODE_BALANCED);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_MOTION_OPTIMIZATION,
		IFACE_TRACK_MOTION_OPTIMIZATION_HISTORY_LONG_FAST);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(objectHandler,
		IFACE_PARAMETER_TRACK_DEEP_TRACK,
		IFACE_TRACK_DEEP_TRACK_DEFAULT);
	error->CheckError(errorCode, error->medium);

	errorCode = IFACE_SetParam(faceHandlerTracking, 
		IFACE_PARAMETER_FACEDET_CONFIDENCE_THRESHOLD, 
		std::to_string(configuration->GetPrecision()).c_str());
	error->CheckError(errorCode, error->medium);
	
	errorCode = IFACE_SetTrackingAreaMask(objectHandler, 
		maskImage, width, height);	
	error->CheckError(errorCode, error->medium);	
	delete[] maskImage;
	
}

void FaceModel::Tracking(vector<unsigned char> buffer, int width, int height) {
	int errorCode;
	unsigned char* rawImage = LoadImage(buffer);


	errorCode = IFACE_TrackObjects(objectHandler, rawImage,
		width, height, countFrameTracking*timeDeltaMs, TRACKED_OBJECTS, objects);
	error->CheckError(errorCode, error->medium);

	countFrameTracking++;


	for (int trackedObjectIndex = 0; trackedObjectIndex < TRACKED_OBJECTS;
		trackedObjectIndex++)
	{		
		float bbX, bbY, bbWidth, bbHeight;
		IFACE_TrackedObjectState trackedState;		

		errorCode = IFACE_GetObjectState(objects[trackedObjectIndex],
			objectHandler, &trackedState);		
		error->CheckError(errorCode, error->medium);

		if (trackedState == IFACE_TRACKED_OBJECT_STATE_CLEAN) {
			
			
			ClearCoordinatesImage(trackedObjectIndex);
			cout << "STATE CLEAN: " << endl;
		}

		/*if (trackedState != IFACE_TRACKED_OBJECT_STATE_CLEAN) {
			errorCode = IFACE_GetObjectId(objects[trackedObjectIndex], objectHandler, &id);
			error->CheckError(errorCode, error->medium);
			cout << "STATE TRACKED ID: " << id << endl;
		}	*/				

		if (trackedState == IFACE_TRACKED_OBJECT_STATE_TRACKED) {			
			errorCode = IFACE_GetObjectBoundingBox(objects[trackedObjectIndex],
				objectHandler, &bbX, &bbY, &bbWidth, &bbHeight);
			error->CheckError(errorCode, error->medium);
			BuildCoordinates(bbX, bbY, bbWidth, bbHeight, trackedObjectIndex);
			cout << "STATE TRACKED: " << endl;
		}

		if (trackedState == IFACE_TRACKED_OBJECT_STATE_COVERED)
		{
			errorCode = IFACE_GetObjectBoundingBox(objects[trackedObjectIndex],
				objectHandler, &bbX, &bbY, &bbWidth, &bbHeight);
			error->CheckError(errorCode, error->medium);
			BuildCoordinates(bbX, bbY, bbWidth, bbHeight, trackedObjectIndex);
		}

		if (trackedState == IFACE_TRACKED_OBJECT_STATE_SUSPEND)
		{
			ClearCoordinatesImage(trackedObjectIndex);
			cout << "STATE SUSPEND INDEX: " << trackedObjectIndex << endl;
		}
	
		if (trackedState == IFACE_TRACKED_OBJECT_STATE_LOST)
		{
			void *newObj;
			errorCode = IFACE_CreateObject(&newObj);
			error->CheckError(errorCode, error->medium);
			objects[trackedObjectIndex] = newObj;
			ClearCoordinatesImage(trackedObjectIndex);
			cout << "STATE LOST: " << endl;
		}

	}

	delete[] rawImage;

}

//int FaceModel::SearchId(int idTracked) {
//	int index = -1;
//	if (coordinatesIdUser.empty())
//	{
//		coordinatesIdUser.push_back(idTracked);
//		return 0;
//	}
//	else {
//		for (unsigned i = 0; i < coordinatesIdUser.size(); i++) {
//			if (coordinatesIdUser[i] == idTracked)
//			{
//				index = coordinatesIdUser.at(i);
//			}
//		}
//			
//	}
//	if (index == -1)
//	{
//		if (coordinatesIdUser.size() == TRACKED_OBJECTS)
//		{
//			coordinatesIdUser.erase(coordinatesIdUser.begin() + idUserSuspend);
//			
//		}
//		coordinatesIdUser.push_back(idTracked);
//		index = coordinatesIdUser.size() - 1;
//	}
//
//	return index;
//}

//void FaceModel::DeleteId(int idTracked) {
//	int index;
//	for (unsigned i = 0; i < coordinatesIdUser.size(); i++) {
//		if (coordinatesIdUser[i] == idTracked)
//		{
//			index = coordinatesIdUser.at(i);
//			
//		}
//	}
//	coordinatesIdUser.erase(coordinatesIdUser.begin() + index);
//	ClearCoordinatesImage(index);
//}

//void FaceModel::SuspendIndex(int idTracked) {
//	int index = 0;
//	for (unsigned i = 0; i < coordinatesIdUser.size(); i++) {
//		if (coordinatesIdUser[i] == idTracked)
//		{
//			index = coordinatesIdUser.at(i);
//
//		}
//	}
//
//	idUserSuspend = index;
//}

void FaceModel::ClearCoordinatesImage(int indexTracked) {
	int index = indexTracked * NUMBER_COORDINATES_IMAGES;
	imageCoordinatesFollowed[index] = 0;
	imageCoordinatesFollowed[index + 1] = 0;
	imageCoordinatesFollowed[index + 2] = 0;
	imageCoordinatesFollowed[index + 3] = 0;
	shootCoordinates.on_next(imageCoordinatesFollowed);
}

void FaceModel::BuildCoordinates(float x, float y, float width, float height, int indexTracked) {
	int index = indexTracked * NUMBER_COORDINATES_IMAGES;
	imageCoordinatesFollowed[index] = x;
	imageCoordinatesFollowed[index + 1] = y;
	imageCoordinatesFollowed[index + 2] = width;
	imageCoordinatesFollowed[index + 3] = height;
	shootCoordinates.on_next(imageCoordinatesFollowed);

}

void FaceModel::FastDetect(unsigned char* rawImage, int width, int height) {
	int detectedFaces = 1;
	int errorCode;
	void* faceHandler;
	void* faceTemp;
	float cropRect[8]; 
	float cropEmptyRect[8];		

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_SetParam(faceHandler,
		IFACE_PARAMETER_FACEDET_SPEED_ACCURACY_MODE,
		IFACE_FACEDET_SPEED_ACCURACY_MODE_FAST);
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_CreateFace(&(faceTemp));
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_DetectFaces(rawImage, width, height,
		configuration->GetMinEyeDistance(), configuration->GetMaxEyeDistance(),
		faceHandler, &detectedFaces, &faceTemp);
	
	if (errorCode == IFACE_OK) {

		if (detectedFaces != EMPTY_FACE)
		{			

			errorCode = IFACE_GetFaceCropRectangle(faceTemp, faceHandler,
				IFACE_FACE_CROP_METHOD_TOKEN_FRONTAL, cropRect);
			error->CheckError(errorCode, error->medium);
			if (errorCode == IFACE_OK) {
				shootCoordinates.on_next(cropRect);

			}
			else {
				shootCoordinates.on_next(cropEmptyRect);
			}

			
		}
		else {
			shootCoordinates.on_next(cropEmptyRect);
		}
				
	}
	else {
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_ReleaseEntity(faceTemp);
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);

}

int FaceModel::ModelOneToOne(vector<unsigned char> buffer, int client) {
	int lenght, width, height, errorCode, templates;
	const char* imgData = reinterpret_cast<const char*> (&buffer[0]);

	if (imgData != NULL) {
		errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &width,
			&height, &lenght, NULL);
		if (errorCode == IFACE_OK)
		{
			
			unsigned char* rawImage = new unsigned char[lenght];
			errorCode = IFACE_LoadImageFromMemory(imgData, (unsigned int)buffer.size(), &width,
				&height, &lenght, rawImage);
			if (errorCode != IFACE_OK)
			{
				error->CheckError(errorCode, error->medium);
			}
			else
			{
				templates = GetOneModel(rawImage, width, height, client);
			}
			delete rawImage;
		}
		else {
			error->CheckError(errorCode, error->medium);
		}
	}

	return templates;
}

unsigned char* FaceModel::LoadFileImage(string image, int *width, int *height, int *length)
{
	int errorCode = IFACE_LoadImage(image.c_str(), width, height, length, NULL);
	if (errorCode != IFACE_OK)
	{
		//printf("  error at loading of image '%s': ", image.c_str());
		error->CheckError(errorCode, error->medium);
		return NULL;
	}

	unsigned char *imageData = new unsigned char[*length];
	errorCode = IFACE_LoadImage(image.c_str(), width, height, length, imageData);
	if (errorCode != IFACE_OK)
	{
		//printf("  error at loading of image '%s': ", image.c_str());
		error->CheckError(errorCode, error->medium);
		return NULL;
	}

	return imageData;
}

void FaceModel::CreateTemplate(void* face, string pathImageCrop, int client) {
	int errorCode;
	int templateSize;
	void* faceHandler;	

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);
	errorCode = IFACE_CreateTemplate(face, faceHandler, 0, &templateSize, NULL);
	if (errorCode == IFACE_OK) {
		char* templateData = new char[templateSize];
		errorCode = IFACE_CreateTemplate(face, faceHandler, 0,
			&templateSize, templateData);
		if (errorCode != IFACE_OK) {
			error->CheckError(errorCode, error->medium);
		}
		else
		{
			//string pathImage = nameDirectory + "/" + nameFileCropImage;
			Molded* model = new Molded();
			model->SetMoldImage(templateData);
			model->SetMoldSize(templateSize);
			model->SetPathImage(pathImageCrop);
			model->SetIdClient(client);
			templateOut.on_next(model);
		}
		delete[] templateData;
	}
	else {
		error->CheckError(errorCode, error->medium);
	}
	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);

}

int FaceModel::GetOneModel(unsigned char* rawImage, 
	int width, int height, int client) {
	int maxFaces = configuration->GetMaxDetect();
	int detectedFaces = configuration->GetMaxDetect();
	int errorCode;
	void** faceTemp = new void*[maxFaces];
	void* faceHandler;

	errorCode = IFACE_CreateFaceHandler(&faceHandler);
	error->CheckError(errorCode, error->medium);

	if (configuration->GetModeDetect() == 1)
	{
		errorCode = IFACE_SetParam(faceHandler,
			IFACE_PARAMETER_FACEDET_SPEED_ACCURACY_MODE,
			IFACE_FACEDET_SPEED_ACCURACY_MODE_BALANCED);
		error->CheckError(errorCode, error->medium);
	}
	if (configuration->GetModeDetect() == 2)
	{
		errorCode = IFACE_SetParam(faceHandler,
			IFACE_PARAMETER_FACEDET_SPEED_ACCURACY_MODE,
			IFACE_FACEDET_SPEED_ACCURACY_MODE_FAST);
		error->CheckError(errorCode, error->medium);
	}	
	
	for (int i = 0; i < maxFaces; i++) { 
		errorCode = IFACE_CreateFace(&(faceTemp[i]));
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_DetectFaces(rawImage, width, height,
		configuration->GetMinEyeDistance(), configuration->GetMaxEyeDistance(), 
		faceHandler, &detectedFaces, faceTemp);
	
	if (errorCode == IFACE_OK) {
		
		if (detectedFaces != EMPTY_FACE)
		{
			for (int i = 0; i < detectedFaces; i++) {
				float rightEyeX, rightEyeY, leftEyeX, leftEyeY;
				float faceConfidence;
				void* face = faceTemp[i];

				errorCode = IFACE_GetFaceBasicInfo(face, faceHandler,
					&rightEyeX, &rightEyeY, &leftEyeX, &leftEyeY, &faceConfidence);
				error->CheckError(errorCode, error->medium);
				cout << "CONFIDENCE: " << faceConfidence << endl;
				if (faceConfidence > configuration->GetPrecision())
				{
					srand((unsigned)time(NULL));
					int randomName = rand() % 100 + 1;
					string pathImage = nameDirectory + "/" + to_string(randomName) + ".png";
					cout << "GREATER OR EQUAL ACCURACY .." << configuration->GetPrecision() << endl;
					std::thread(&FaceModel::FaceCropImage, this,
						face, pathImage).detach();
					CreateTemplate(face, pathImage, client);
				}
			}
			
		}
	}
	else {
		error->CheckError(errorCode, error->medium);
	}
	for (int i = 0; i < maxFaces; i++) { 
		errorCode = IFACE_ReleaseEntity(faceTemp[i]);
		error->CheckError(errorCode, error->medium);
	}

	errorCode = IFACE_ReleaseEntity(faceHandler);
	error->CheckError(errorCode, error->medium);

	return detectedFaces;
}

