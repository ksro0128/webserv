#pragma once

#include "../include.hpp"
#include "Server.hpp"

class Config
{
	public:
		Config();
		Config(const Config&);
		Config& operator=(const Config&);
		~Config();
		void PrintConfig();
		void ParseConfig(std::string path);
		std::vector<int> GetPorts();
		std::string GetMimeType(std::string extension);
		std::string GetStatusMessage(int status);
		std::string GetExtension(std::string path);
		

		std::string& GetRoot(int port, std::string serverName, std::string path);
		std::string GetFilePath(int port, std::string serverName, std::string path);
		std::vector<std::string>& GetMethods(int port, std::string serverName, std::string path);
		std::string GetCgiPath(int port, std::string serverName, std::string path);
		int GetRedirectStatus(int port, std::string serverName, std::string path);
		std::string GetRedirectionPath(int port, std::string serverName, std::string path);
		std::string GetErrorPage(int port, std::string serverName, int status);
		int GetLimitBodySize(int port, std::string serverName, std::string path);
		bool IsAutoIndex(int port, std::string serverName, std::string path);
		std::vector<std::string>& GetIndexes(int port, std::string serverName, std::string path);

	private:
		std::string setConfigString(std::string filestring);
		std::string addSpaceAroundChar(std::string filestring, char c);

		std::multimap<int, Server> mServer;
		std::map<std::string, std::string> mMimeSet;
		std::map<int, std::string> mStatusMessageSet;
		void parseMime();

		Server& getServer(int port, std::string serverName);
		std::string makeErrorPage(int status, std::string statusMessage);
};