#pragma once

#include "include.hpp"
#include "Server.hpp"

class Config
{
	public:
		Config();
		Config(const Config&);
		Config& operator=(const Config&);
		~Config();
		void ParseConfig(std::string path);
		Server& GetServer(int port, std::string serverName);
		std::vector<int> GetPorts();
		std::string GetMimeType(std::string extension);
		std::string GetStatusMessage(int status);
		std::string GetExtension(std::string path);
	private:
		std::multimap<int, Server> m_configMultiMap;
		std::string ignoreComment(std::string filestring);
		std::string semicolonToSpaceSemicolonSpace(std::string filestring);
		std::string braceToSpaceBraceSpace(std::string filestring);
		std::map<std::string, std::string> m_mimeSet;
		std::map<int, std::string> m_statusMessageSet;
		void parseMime();
};