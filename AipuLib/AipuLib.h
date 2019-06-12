#pragma once
#ifdef AIPULIB_EXPORTS
#define AIPULIB_API __declspec(dllexport)
#else
#define AIPULIB_API __declspec(dllimport)
#endif

#include "Management.h"
#include "Innovatrics.h"

class AIPULIB_API AipuLib
{
public:
	AipuLib();
	~AipuLib();
	Rx::subject<string> errorSubject;
	Rx::observable<string> observableError = errorSubject.get_observable();

	Rx::subject<Mat> frame;
	Rx::observable<Mat> observableFrame = frame.get_observable();

	Rx::subject<string> userDetected;
	Rx::observable<string> observableUserJSON = userDetected.get_observable();

	void InitLibrary();
	void LoadConfiguration(string nameFile);
	void RunVideo();
	
private:
	Rx::subscriber<string> shootError = errorSubject.get_subscriber();
	Rx::subscriber<Mat> frameOut = frame.get_subscriber();
	Rx::subscriber<string> shootUserJSON = userDetected.get_subscriber();
	Management* management;
	Innovatrics* innovatrics = new Innovatrics();
	void ObserverError();
};

