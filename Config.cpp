#include "Config.hpp"

Config::Config()
{
	
}

Config::Config(const Config& rhs)
{
	_configMultiMap = rhs._configMultiMap;
}

Config& Config::operator=(const Config& rhs)
{
	if (this == &rhs)
		return *this;
	_configMultiMap = rhs._configMultiMap;
	return *this;
}

Config::~Config()
{
}

void Config::parseConfig(std::string path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		throw std::runtime_error("Error: cannot open config file");
	}
	std::ostringstream fileStream;
	fileStream << file.rdbuf();
	std::string fileString = fileStream.str();
	fileString = _ignoreComment(fileString);
	fileString = _semicolonToSpaceSemicolonSpace(fileString);
	fileString = _braceToSpaceBraceSpace(fileString);
	std::istringstream iss(fileString);
	std::string line;
	while (std::getline(iss, line))
	{
		std::istringstream issLine(line);
		std::string token;
		issLine >> token;
		if (token.size() == 0)
			continue;
		if (token == "server")
		{
			issLine >> token;
			if (token != "server")
			{
				throw std::runtime_error("Error: config file is invalid\nline: " + line + "\n");
			}
			std::getline(iss, line);
			issLine.clear();
			issLine.str(line);
			if (issLine >> token && token == "{")
			{
				issLine >> token;
				if (token != "{")
				{
					throw std::runtime_error("Error: config file is invalid\nline: " + line + "\n");
				}
				int cnt = 0;
				std::string block;
				while (std::getline(iss, line))
				{
					issLine.clear();
					issLine.str(line);
					while (issLine >> token)
					{
						if (token == "{")
							cnt++;
						if (token == "}")
							cnt--;
					}
					if (cnt < 0)
						break;
					block += line + "\n";
				}
				if (token != "}")
				{
					throw std::runtime_error("Error: config file is invalid\nline: " + line + "\n");
				}
				Server server;
				server.parseServer(block);
				for (std::vector<int>::iterator it = server.getPort().begin(); it != server.getPort().end(); it++)
				{
					_configMultiMap.insert(std::pair<int, Server>(*it, server));
				}
			}
			else
			{
				throw std::runtime_error("Error: config file is invalid\nline: " + line + "\n");
			}
		}
		else
		{
			throw std::runtime_error("Error: config file is invalid\nline: " + line + "\n");
		}
	}
	
}


std::string Config::_ignoreComment(std::string fileString)
{
	std::string::size_type pos = fileString.find("#");
	while (pos != std::string::npos)
	{
		std::string::size_type end = fileString.find("\n", pos);
		fileString.erase(pos, end - pos);
		pos = fileString.find("#");
	}
	return fileString;
}

std::string Config::_semicolonToSpaceSemicolonSpace(std::string fileString)
{
	std::string::size_type pos = fileString.find(";");
	while (pos != std::string::npos)
	{
		fileString.replace(pos, 1, " ; ");
		pos = fileString.find(";", pos + 3);
	}
	return fileString;
}

std::string Config::_braceToSpaceBraceSpace(std::string fileString)
{
	std::string::size_type pos = fileString.find("{");
	while (pos != std::string::npos)
	{
		fileString.replace(pos, 1, " { ");
		pos = fileString.find("{", pos + 3);
	}
	pos = fileString.find("}");
	while (pos != std::string::npos)
	{
		fileString.replace(pos, 1, " } ");
		pos = fileString.find("}", pos + 3);
	}
	return fileString;
}

Server& Config::getServer(int port, std::string serverName)
{
	std::multimap<int, Server>::iterator it = _configMultiMap.find(port);
	if (it == _configMultiMap.end())
	{
		throw std::runtime_error("Error: server not found");
	}
	while (it != _configMultiMap.end() && it->first == port)
	{
		for (std::vector<std::string>::iterator it2 = it->second.getServerName().begin(); it2 != it->second.getServerName().end(); it2++)
		{
			if (*it2 == serverName)
			{
				return it->second;
			}
		}
		it++;
	}
	it = _configMultiMap.find(port);
	return it->second;
}

std::vector<int> Config::getPorts()
{
	std::vector<int> ports;
	for (std::multimap<int, Server>::iterator it = _configMultiMap.begin(); it != _configMultiMap.end(); it++)
	{
		if (std::find(ports.begin(), ports.end(), it->first) == ports.end())
			ports.push_back(it->first);
	}
	return ports;
}