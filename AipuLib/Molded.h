#ifndef Molded_h
#define Molded_h

#include <iostream>

using namespace std;

class Molded
{
public:
	Molded();
	~Molded();
	void SetMoldImage(char* mold) {
		moldImage = mold;
	}

	void SetMoldSize(int size) {
		moldSize = size;
	}

	char* GetMoldImage() {
		return moldImage;
	}

	int GetMoldSize() {
		return moldSize;
	}

	void SetPathImage(string path) {
		pathImage = path;
	}

	string GetPathImage() {
		return pathImage;
	}

private:
	char* moldImage;
	int moldSize;
	string pathImage;
};


#endif // !Molded_h

