#ifndef FlowTrend_h
#define FlowTrend_h

#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include "ErrorAipuLib.h"
#include <fstream>
#include <thread>
#include <ctime>

#define FLOW_LIMIT		1000
#define TWO_VARIABLES	2

using namespace Eigen;
using namespace std;

class FlowTrend
{
public:
	FlowTrend();
	~FlowTrend();
	void SetDirectory(string nameDirectory) {
		directory = nameDirectory;
	}
	void Init();
	int GetFlowTrendForMinute(int minute);
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
private:
	const string LABEL_ERROR_NOT_FILE_ML = "Not file machine learning";
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	ErrorAipuLib* error = new ErrorAipuLib();
	void Split(const string& source, char separator,
		vector<string>& pieces);
	void LoadMatrix();
	bool LoadDataOfFile(string nameFile);
	void NormalEquations();
	void RefreshData(string nameFile);
	void UpdateTrendByHour();
	void ObserverError();
	vector<int> quantityDetected;
	vector<int> dayOfDetected;
	vector<int> minuteOfDetected;
	MatrixXf X;
	VectorXf y;
	VectorXf theta;
	int hourNow = 0;
	string directory;
};


#endif // !FlowTrend_h

