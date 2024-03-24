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
		void parseConfig(std::string path);
		Server& getServer(int port, std::string serverName);
		std::vector<int> getPorts();
	private:
		std::multimap<int, Server> _configMultiMap;
		std::string _ignoreComment(std::string filestring);
		std::string _semicolonToSpaceSemicolonSpace(std::string filestring);
		std::string _braceToSpaceBraceSpace(std::string filestring);
};