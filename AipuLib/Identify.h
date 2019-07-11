#ifndef Identify_h
#define Identify_h

#include "idkit.h"
#include "ErrorIdKitLib.h"
#include "Molded.h"
#include "User.h"

class Identify
{
public:
	Identify();
	~Identify();
	void LoadConnection();	
	void EnrollUser(Molded* modelImage);
	void SetIsRegister(bool option) {
		isRegister = option;
	}

	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	Rx::subject<User*> userSubject;
	Rx::observable<User*> observableUser = userSubject.get_observable();

private:
	ErrorIdKitLib* error = new ErrorIdKitLib();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	Rx::subscriber<User*> shootUser = userSubject.get_subscriber();	
	bool isRegister = true;
	void ObserverError();
};


#endif // !Identify_h

