#include "Innovatrics.h"

Innovatrics::Innovatrics()
{
	ObserverError();
	std::thread tvg(&Innovatrics::VerifyProcessorGraphic, this);
	tvg.detach();
}

Innovatrics::~Innovatrics()
{
	IFACE_Terminate();

}

void Innovatrics::VerifyProcessorGraphic()
{
	isGraphicProcessor = graphicProcessor->ThereIsGraphicProcessor();
}

void Innovatrics::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {		
		shootError.on_next(either);
	});
}

void Innovatrics::InitLibrary() {
	int errorCode;

	errorCode = IFACE_Init();
	if (errorCode != IFACE_OK) {		
		error->CheckError(errorCode, error->gross);
	}	
	
}

bool Innovatrics::InitParamsGraphicProcessor() {
	int errorCode;
	errorCode = IFACE_SetParam(nullptr,
		IFACE_PARAMETER_GLOBAL_GPU_ENABLED, "true");
	if (errorCode == IFACE_OK) {
		errorCode = IFACE_SetParam(nullptr,
			IFACE_PARAMETER_GLOBAL_GPU_DEVICE_ID, "0");
		if (errorCode != IFACE_OK) {
			error->CheckError(errorCode, error->less);
			return false;
		}
	}
	else {
		error->CheckError(errorCode, error->less);
		return false;
	}
	return true;
}

void Innovatrics::SetParamsLibrary() {
	int errorCode;
	if (!isGraphicProcessor || !InitParamsGraphicProcessor())
	{
		errorCode = IFACE_SetParam(IFACE_GLOBAL_PARAMETERS,
			IFACE_PARAMETER_GLOBAL_THREAD_NUM, IFACE_GLOBAL_THREAD_NUM_DEFAULT);
		if (errorCode != IFACE_OK) {
			error->CheckError(errorCode, error->medium);
		}
		cout << "CUDA SI EXISTE.." << endl;
	}
	else {
		cout << "CUDA NO EXISTE.." << endl;
	}
	
}

void Innovatrics::Terminate() {
	int errorCode;

	errorCode = IFACE_Terminate();
	if (errorCode != IFACE_OK) {
		error->CheckError(errorCode, error->gross);
	}
}