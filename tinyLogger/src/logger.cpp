#include "conf.h"
#include "logger.h"

#include <regex>

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include <string.h>

#include <fstream>
#include <sstream>

#define TINYLOGGER_SERVER_VERSION "1.0"

TinyLogger::TinyLogger()
{
	//server configuration reading
	SettingReader serverConfig;
	serverConfig.ReadConfiguration();

	if (serverConfig.config.find("address") != serverConfig.config.end())
	{
		setting["address"] = serverConfig.config["address"];
	}
	if (serverConfig.config.find("port") != serverConfig.config.end())
	{
		setting["port"] = serverConfig.config["port"];
	}
}

int TinyLogger::AquireFacilities()
{
	DIR *facilitiesDir = opendir("/etc/tinyLogger/facilities/");
	if (facilitiesDir == NULL)
	{
		return -1;
	}

	int filesNum = 0;
	dirent *dirInfo = NULL;
	while((dirInfo = readdir(facilitiesDir)) != NULL)
	{
		std::string fileName("/etc/tinyLogger/facilities/");
		fileName+= dirInfo->d_name;
		if ((fileName.compare(".") == 0) || (fileName.compare("..") == 0) || (dirInfo->d_type != DT_REG))
		{
			continue;
		}

		if ((fileName.length() < 6) || (fileName.compare(fileName.length() - 5, 5, ".conf") != 0))
		{
			continue;
		}
		SettingReader config(fileName);
		config.ReadConfiguration();


		services[config.config["name"]] = config.config;

		++filesNum;
	}
	return filesNum;
}

std::string TinyLogger::EventHandler(std::string &buffer)
{
	//The lesser you think, the better life will be.
	//So there is no code for corrupted packet processing.
	std::string response;

	while (true)
	{
		//conetent incomplete
		size_t headersEnd = buffer.find("\r\n\r\n");
		if (headersEnd == std::string::npos)
		{
			return response;
		}
	
		std::string headersString(buffer, 0, headersEnd + 2);
	
		std::map<std::string, std::string> headers = ProtocolParser(headersString);

		/* 
		   if encountered a bad contentLength(like data_length: shit), 
		   let stoi() raise exception, and the Network::StartHandleRequest() will catch this exception, close this connection.
		*/
		long contentLength = std::stol(headers["data_length"]);
		if (contentLength <= 0) throw std::runtime_error("?");
		if ((headersEnd + contentLength) > buffer.length())
		{
			return "";
		}

		if (headers["app_key"] != services[headers["app_name"]]["auth_key"])
		{
			buffer = buffer.substr(headersEnd + contentLength);
			response += CreateResponseMsg("403 Confidential Mismatch");
			continue;
		}
		std::string contentBytes(buffer, headersEnd + 4, contentLength);
		buffer = buffer.substr(headersEnd + contentLength);
		if(!WriteLog(headers["app_name"], contentBytes))
		{
			response += CreateResponseMsg("500 Write Failed");
		}
		else
		{
			response += CreateResponseMsg("200 OK");
		}	
	}
}

std::map<std::string, std::string> TinyLogger::ProtocolParser(const std::string headersBytes)
{
	std::map<std::string, std::string> headers;
	size_t startPos = 0;
	size_t endPos = 0;
	for (endPos = headersBytes.find("\r\n"); endPos != headersBytes.npos; endPos = headersBytes.find("\r\n", endPos + 1))
	{
		std::string header(headersBytes.substr(startPos, endPos - startPos));
		startPos = endPos + 2;

		size_t splitPos = header.find(": ");
		if (splitPos == header.npos) continue;

		std::string key(header, 0, splitPos);
		std::string value(header, splitPos + 2, endPos - (splitPos + 2));

		headers[key] = value;
	}
	return headers;
}

bool TinyLogger::WriteLog(std::string appName, std::string content)
{
	using std::fstream;
	std::string filePath = services[appName]["file_path"] + services[appName]["name"] + ".log";
	fstream logFile(filePath, fstream::out | fstream::app);
	if (!logFile.good())
	{
		return false;
	}

	logFile << content << std::flush;
	logFile.close();
	return true;
}

std::string TinyLogger::CreateResponseMsg(std::string msg)
{
	std::stringstream response;

	response << "version: " << TINYLOGGER_SERVER_VERSION << "\r\n";
	response << "data_length: " << msg.length() << "\r\n";
	response << "server: " << getpid() << "\r\n";
	response << "\r\n";
	response << msg;

	return response.str(); 
}
