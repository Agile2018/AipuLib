#ifndef User_h
#define User_h

#include <iostream>

using namespace std;
class User
{
public:
	User();
	~User();
	const string fieldUserIdFace = "id_face";
	const string fieldNameUser = "name";
	const string fieldAddressUser = "address";
	const string fieldId = "_id";
	const string fieldClient = "client";

	void SetIdDb(string id) {
		idDb = id;
	}
	void SetUserIdIFace(int id) {
		userIdIFace = id;
	}

	void SetNameUser(string name) {
		nameUser = name;
	}

	void SetAddressUser(string address) {
		addressUser = address;
	}
	string GetIdDb() {
		return idDb;
	}
	int GetUserIdIFace() {
		return userIdIFace;
	}

	string GetNameUser() {
		return nameUser;
	}

	string GetAddressUser() {
		return addressUser;
	}

	void SetPathImageTemp(string path) {
		pathImageTemp = path;
	}

	string GetPathImageTemp() {
		return pathImageTemp;
	}

	void SetIsNew(bool response) {
		isNew = response;
	}

	bool GetIsNew() {
		return isNew;
	}

	void SetClient(int id) {
		client = id;
	}
	int GetClient() {
		return client;
	}

private:
	string idDb;
	int userIdIFace;	
	string nameUser;
	string addressUser;
	string pathImageTemp;
	bool isNew = false;
	int client;
};


#endif // !User_h

