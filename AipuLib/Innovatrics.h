#ifndef Innovatrics_h
#define Innovatrics_h

#include "ErrorFaceLib.h"
#include "iface.h"
#include "idkit.h"

class Innovatrics
{
public:
	Innovatrics();
	~Innovatrics();
	void InitLibrary();
	void SetIsGraphicProcessor(bool processor) {
		isGraphicProcessor = processor;
	}
	void SetParamsLibrary();
	
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();	

private:
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	ErrorFaceLib* error = new ErrorFaceLib();
	bool InitParamsGraphicProcessor();
	bool isGraphicProcessor = false;
	void ObserverError();
};


#endif // !Innovatrics_h

