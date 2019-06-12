#ifndef ErrorFaceLib_h
#define ErrorFaceLib_h


#include "iface.h"
#include "rx.hpp"
#include "CodesError.h"
#include "Either.h"
#define BUFFER_LENGTH	1024
#define CHAR_LENGTH		256
namespace Rx {
	using namespace rxcpp;
	using namespace rxcpp::subjects;
	using namespace rxcpp::operators;
	using namespace rxcpp::util;

}

class ErrorFaceLib
{
public:
	ErrorFaceLib();
	~ErrorFaceLib();
	enum ErrorWeight
	{
		gross = GROSS_ERROR,
		medium = MEDIUM_ERROR,
		less = LESS_ERROR,
		none = PROCESS_OK
	};
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
	void CheckError(int errorCode, ErrorWeight errorWeight);

private:	
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	void BuildMessageErrorLicense(int errorCode, ErrorWeight errorWeight);
	void BuildMessageOtherError(int errorCode, ErrorWeight errorWeight);
	void BuildMessageOk();
};

#endif // !ErrorFaceLib_h

