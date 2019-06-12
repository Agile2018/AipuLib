#include "Train.h"

Train::Train()
{
}

Train::~Train()
{
}

void Train::CreateTrainingDirectory(string directory) {
	_mkdir((char *)&directory);
	directoryTrain = directory;
}

void Train::WriteDataTraining(string nameFile) {
	string path = directoryTrain + "/" + nameFile;
	ofstream file_obj;
	file_obj.open(path, ios::app);
	file_obj << minuteOfDay << "," << quantityOfImages << endl;
	file_obj.close();

}