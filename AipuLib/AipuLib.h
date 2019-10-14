#pragma once
#ifdef AIPULIB_EXPORTS
#define AIPULIB_API __declspec(dllexport)
#else
#define AIPULIB_API __declspec(dllimport)
#endif

#include <string>

using namespace std;


class AIPULIB_API AipuLib
{
public:
	AipuLib();
	~AipuLib();
	
	void InitLibrary();
	void LoadConfiguration(string nameFile);
	
	string GetUserJSON();
	string GetMessageError();
	float* GetCoordinates();
	void SetIsRegister(bool option);
	bool GetStateProccessRecognition();
	void ResetIdUser();
	void SetWorkMode(int mode);
	int GetWorkMode();
	void RecognitionFace(unsigned char* image, int rows, int cols, int client);
	void RecognitionFastFace(unsigned char* image, int rows, int cols);
	void InitTracking(unsigned char* image, int rows, int cols);
	void Tracking(unsigned char* image, int rows, int cols);
	void Terminate(int option);
	void Reset();
	void SetSequenceFps(int fps);
	void ResfreshBetweenFrame(int refresh);
	void SetConfigurationDatabase();
	void TerminateTracking();
	void ResetLowScore();
	int GetCountLowScore();
	void ResetCountRepeatUser();
	int GetCountRepeatUser();
	void ResetCountNotDetect();
	int GetCountNotDetect();
private:	
	
	void ObserverEvent();

};

