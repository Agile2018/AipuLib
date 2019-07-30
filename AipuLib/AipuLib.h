#pragma once
#ifdef AIPULIB_EXPORTS
#define AIPULIB_API __declspec(dllexport)
#else
#define AIPULIB_API __declspec(dllimport)
#endif

#include <string>
//#include <vector>
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
	void SetIsRegister(bool option);
	
	void SetWorkMode(int mode);
	int GetWorkMode();
	void RecognitionFace(unsigned char* image, int rows, int cols);
	void Terminate();
	
private:	
	
	void ObserverError();

};

