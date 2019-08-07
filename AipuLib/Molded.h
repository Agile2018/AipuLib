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

	int GetIdClient() {
		return idClient;
	}

	void SetIdClient(int id) {
		idClient = id;
	}
private:
	char* moldImage;
	int moldSize;
	string pathImage;
	int idClient;
};


#endif // !Molded_h

