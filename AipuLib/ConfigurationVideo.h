#ifndef ConfigurationVideo_h
#define ConfigurationVideo_h

#define NOT_DEVICE -1
#define FPS  30
#define MILI_SECONDS  1000
#define BATCH_TOTAL_SIZE  30

#include "Configuration.h"
#include "ErrorAipuLib.h"
#include "LabelError.h"
#include <cctype>

class ConfigurationVideo: public Configuration
{
public:
	ConfigurationVideo();
	~ConfigurationVideo();
	void ParseJSONToObject();
	void ParseMapToJSON();
	void SetPath(string valuePath);
	void SetUser(string valueUser);
	void SetPassword(string valuePassword);
	void SetResolution(string valueResolution);
	void SetFPS(string valueFps);	
	void SetFPSIntToString(int intFPS);
	void SetImageTemp(string name);	
	string GetPath();
	string GetUser();
	string GetPassword();
	string GetResolution();
	string GetFPS();
	int GetFPSStringToInt();
	string GetImageTemp();
	int GetDeviceId();
	string GetURLVideo();
	bool BuildPathVideo();
	Rx::subject<Either*> errorSubject;
	Rx::observable<Either*> observableError = errorSubject.get_observable();
private:
	ErrorAipuLib* error = new ErrorAipuLib();
	Rx::subscriber<Either*> shootError = errorSubject.get_subscriber();
	const string PATH = "path";
	const string PASSWORD = "password";
	const string USER = "user";
	const string RESOLUTION = "resolution";
	const string FRAMEXSECONDS = "fps";
	const string IMAGETEMP = "imagetemp";
	const string NAMEWINDOW = "namewindow";
	const string PARAMS = "Params";
	const string CONFIGURATION = "configuration";
	const string VIDEO_CONFIGURATION = "video_configuration";
	const string HTTP = "http://";
	const string RTSP = "rtsp://";	
	string path = "";
	string user = "";
	string password = "";
	string resolution = "";
	string fps = ""; //frames x second
	string imageTemp = "";
	int deviceId = NOT_DEVICE;
	string urlVideo;
	void SetValueJSONToConfiguration();
	string BuildUniformResourceLocator(string address);
	bool IsLocalDevice(string device);
	bool HasProtocol(string address);
	void ObserverError();
};


#endif // !ConfigurationVideo_h

