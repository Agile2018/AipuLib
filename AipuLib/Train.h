#ifndef Train_h
#define Train_h

#include <iostream>
#include <fstream> 
#include <direct.h>

using namespace std;

class Train
{
public:
	Train();
	~Train();
	void CreateTrainingDirectory(string directory);
	void WriteDataTraining(string nameFile);
	void SetMinute(int minute) {
		minuteOfDay = minute;
	}

	void SetQuantityOfImages(int quantity) {
		quantityOfImages = quantity;
	}

	void SetDayOfWeek(int day) {
		dayOfWeek = day;
	}

private:
	int minuteOfDay;
	int quantityOfImages;
	int dayOfWeek;
	string directoryTrain;
};


#endif // !Train_h

