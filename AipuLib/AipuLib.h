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
	void RunVideo();
	string GetUserJSON();
	string GetMessageError();
	void SetIsRegister(bool option);
	void StopVideo();
	void SetLapseFrameToFrame(int lapse);
	void SetIndexImage(int index);

	/*void GetFaceMat(int &channels, int &width,
		int &height, int &step1, vector<unsigned char> &data,
		int &cols, int &rows, int &elemSize1, int &step);*/
	
private:	
	
	void ObserverError();

};

