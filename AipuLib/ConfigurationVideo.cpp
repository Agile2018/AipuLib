#include "ConfigurationVideo.h"

ConfigurationVideo::ConfigurationVideo()
{
	ObserverError();
}

ConfigurationVideo::~ConfigurationVideo()
{
}

void ConfigurationVideo::ObserverError() {
	auto observerError = error->observableError.map([](Either* either) {
		return either;
	});

	auto subscriptionError = observerError.subscribe([this](Either* either) {		
		shootError.on_next(either);
	});
}

void ConfigurationVideo::SetPath(string valuePath) {
	path = valuePath;
}

string ConfigurationVideo::GetPath() {
	return path;
}

void ConfigurationVideo::SetUser(string valueUser) {
	user = valueUser;
}

string ConfigurationVideo::GetUser() {
	return user;
}

void ConfigurationVideo::SetPassword(string valuePassword) {
	password = valuePassword;
}

string ConfigurationVideo::GetPassword() {
	return password;
}

void ConfigurationVideo::SetResolution(string valueResolution) {
	resolution = valueResolution;
}

string ConfigurationVideo::GetResolution() {
	return resolution;
}

void ConfigurationVideo::SetFPS(string valueFps) {
	fps = valueFps;
}

string ConfigurationVideo::GetFPS() {
	return fps;
}

void ConfigurationVideo::SetImageTemp(string name) {
	imageTemp = name;
}

string ConfigurationVideo::GetImageTemp() {
	return imageTemp;
}

void ConfigurationVideo::SetValueJSONToConfiguration() {
	const Json::Value params = jsonBody[PARAMS];
	for (Json::Value::const_iterator it = params.begin();
		it != params.end(); ++it) {
		
		if (it.key().asString() == PATH) {
			path = it->asString();
		}
		else if (it.key().asString() == PASSWORD) {
			if (!it->asString().empty()) {
				password = it->asString();
			}
		}
		else if (it.key().asString() == USER) {
			if (!it->asString().empty()) {
				user = it->asString();
			}
		}
		else if (it.key().asString() == RESOLUTION) {
			if (!it->asString().empty()) {
				resolution = it->asString();
			}
		}
		else if (it.key().asString() == FRAMEXSECONDS) {
			if (!it->asString().empty()) {
				fps = it->asString();
			}
		}
		else if (it.key().asString() == IMAGETEMP) {
			imageTemp = it->asString();
		}
	}

}

void ConfigurationVideo::ParseJSONToObject() {
	jsonBody.clear();
	stringJSON = manageFile->ReadFileText();
	bool parsingSuccessful = reader.parse(stringJSON, jsonBody);
	if (parsingSuccessful)
	{
		SetValueJSONToConfiguration();
	}
	else {
		error->CheckError(ERROR_PARSING, error->medium, LABEL_ERROR_PARSING);
	}

}

void ConfigurationVideo::ParseMapToJSON() {
	jsonBody.clear();
	jsonParams.clear();
	std::map<std::string, std::string> params;
	params.insert(std::pair<std::string, std::string>(PATH, path));
	params.insert(std::pair<std::string, std::string>(PASSWORD, password));
	params.insert(std::pair<std::string, std::string>(USER, user));
	params.insert(std::pair<std::string, std::string>(RESOLUTION, resolution));
	params.insert(std::pair<std::string, std::string>(FRAMEXSECONDS, fps));
	params.insert(std::pair<std::string, std::string>(IMAGETEMP, imageTemp));
	std::map<std::string, std::string>::const_iterator it = params.begin(),
		end = params.end();
	for (; it != end; ++it) {
		jsonParams[it->first] = it->second;

	}
	
	jsonBody[CONFIGURATION] = VIDEO_CONFIGURATION;
	jsonBody[PARAMS] = jsonParams;

	SaveConfiguration();

}

string ConfigurationVideo::BuildUniformResourceLocator(string address) {
	string url;
	string protocol = address.substr(0, 7);

	address.replace(0, 7, "");
	if (protocol == HTTP)
	{
		url = protocol + GetUser() + ":" +
			GetPassword() + "@" +
			address + "/axis-cgi/mjpg/video.cgi?resolution=" +
			GetResolution() + "&req_fps=30&.mjpg";

	}
	if (protocol == RTSP)
	{
		url = protocol + GetUser() + ":" +
			GetPassword() + "@" +
			address;

	}

	return url;

}

bool ConfigurationVideo::IsLocalDevice(string device) {
	return !device.empty() &&
		(std::count_if(device.begin(), device.end(), std::isdigit) == device.size());

}

bool ConfigurationVideo::HasProtocol(string address) {

	if (address.find(HTTP) == string::npos &&
		address.find(RTSP) == string::npos) {

		return false;
	}
	else {
		return true;
	}

}

int ConfigurationVideo::GetDeviceId() {
	return deviceId;
}

string ConfigurationVideo::GetURLVideo() {
	return urlVideo;
}

bool ConfigurationVideo::BuildPathVideo() {
	string path = GetPath();
	
	if (!path.empty()) {
		if (IsLocalDevice(path)) {
			deviceId = stoi(path);
		}
		else if (HasProtocol(path)) {
			urlVideo = BuildUniformResourceLocator(path);
		}
		else {
			urlVideo = path;
		}
		return true;
	}
	else {
		return false;
	}

}

void ConfigurationVideo::SetFPSIntToString(int intFPS) {	
	std::stringstream stringFPS;
	stringFPS << intFPS;
	fps = stringFPS.str();
}

int ConfigurationVideo::GetFPSStringToInt() {
	int intFPS;
	std::istringstream stringToInt(fps);
	stringToInt >> intFPS;
	return intFPS;
}