#include "FlowTrend.h"

FlowTrend::FlowTrend()
{
	ObserverError();
}

FlowTrend::~FlowTrend()
{
}

void FlowTrend::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {					
		shootError.on_next(either);
	});
}

void FlowTrend::Split(const string& source, char separator,
	vector<string>& pieces) {
	string::size_type i = 0;
	string::size_type j = source.find(separator);

	while (j != string::npos) {
		pieces.push_back(source.substr(i, j - i));
		i = ++j;
		j = source.find(separator, j);

		if (j == string::npos)
			pieces.push_back(source.substr(i, source.length()));
	}

}

void FlowTrend::LoadMatrix() {
	try
	{
		if (quantityDetected.size() > 0) {
			MatrixXf z(quantityDetected.size(), TWO_VARIABLES);
			VectorXf w(quantityDetected.size());
			X = z;
			y = w;
			for (unsigned int i = 0; i < quantityDetected.size(); i++) {
				X(i, 0) = 1;
				//X(i, 1) = static_cast<float>(dayOfDetected[i]);
				X(i, 1) = static_cast<float>(minuteOfDetected[i]);
				y(i) = static_cast<float>(quantityDetected[i]);
			}
		}
	}
	catch (const std::exception& e)
	{
		error->CheckError(ERROR_MACHINE_LEARNING,
			error->medium, e.what());
	}

}

bool FlowTrend::LoadDataOfFile(string nameFile) {

	try
	{
		dayOfDetected.clear();
		minuteOfDetected.clear();
		quantityDetected.clear();

		ifstream data(nameFile, std::ifstream::in);
		if (data.good()) {
			string row;
			while (data >> row && !row.empty()) {
				//if (!row.empty())
				//{
				vector<string> pieces;
				Split(row, ',', pieces);
				/*stringstream geek(pieces[0]);
				int x = 0;
				geek >> x;
				dayOfDetected.push_back(x);*/
				stringstream geek1(pieces[0]);
				int w = 0;
				geek1 >> w;
				minuteOfDetected.push_back(w);
				stringstream geek2(pieces[1]);
				int z = 0;
				geek2 >> z;
				quantityDetected.push_back(z);
				//}
			}
			data.close();
			return true;
		}
		else
		{			
			error->CheckError(ERROR_MACHINE_LEARNING,
				error->medium, LABEL_ERROR_NOT_FILE_ML);
		}

	}
	catch (const std::exception& e)
	{
		
		error->CheckError(ERROR_MACHINE_LEARNING,
			error->medium, e.what());
	}

	return false;

}

void FlowTrend::NormalEquations() {
	try
	{
		VectorXf t = (X.transpose() * X).ldlt().solve(X.transpose() * y);
		theta = t;

	}
	catch (const std::exception& e)
	{
		error->CheckError(ERROR_MACHINE_LEARNING,
			error->medium, e.what());

	}

}

void FlowTrend::RefreshData(string nameFile) {
	
	if (LoadDataOfFile(nameFile)) {
		LoadMatrix();
		NormalEquations();
	}

}

void FlowTrend::UpdateTrendByHour() {
	
	std::time_t rawtime = std::time(nullptr);	
	tm *timeinfo = std::localtime(&rawtime);
	//struct tm *timeinfo;
	//tm *ltm = std::localtime(&rawtime);
	//localtime_s(timeinfo, &rawtime);
	int hour = timeinfo->tm_hour;
	int day = timeinfo->tm_wday;
	
	if (hourNow < hour)
	{
		hourNow = hour;
		string fileName = directory + "/" + to_string(day) + "-" + to_string(hour) + ".txt";
		RefreshData(fileName);
	}

}

void FlowTrend::Init() {
	UpdateTrendByHour();
}

int FlowTrend::GetFlowTrendForMinute(int minute) {
	std::thread(&FlowTrend::UpdateTrendByHour, this).detach();
	if (theta.size() != 0)
	{
		VectorXf params(TWO_VARIABLES);
		params(0) = 1;
		//params(1) = static_cast<float>(day);
		params(1) = static_cast<float>(minute);
		float foreCast = params.transpose() * theta;
		return static_cast<int>(foreCast);
	}
	return FLOW_LIMIT;

}