#include "Config.hpp"

Config::Config()
{
	m_statusMessageSet[100] = "Continue";
	m_statusMessageSet[101] = "Switching Protocols";
	m_statusMessageSet[200] = "OK";
	m_statusMessageSet[201] = "Created";
	m_statusMessageSet[202] = "Accepted";
	m_statusMessageSet[204] = "No Content";
	m_statusMessageSet[206] = "Partial Content";
	m_statusMessageSet[300] = "Multiple Choices";
	m_statusMessageSet[301] = "Moved Permanently";
	m_statusMessageSet[302] = "Found";
	m_statusMessageSet[303] = "See Other";
	m_statusMessageSet[304] = "Not Modified";
	m_statusMessageSet[400] = "Bad Request";
	m_statusMessageSet[401] = "Unauthorized";
	m_statusMessageSet[403] = "Forbidden";
	m_statusMessageSet[404] = "Not Found";
	m_statusMessageSet[405] = "Method Not Allowed";
	m_statusMessageSet[406] = "Not Acceptable";
	m_statusMessageSet[408] = "Request Timeout";
	m_statusMessageSet[409] = "Conflict";
	m_statusMessageSet[411] = "Length Required";
	m_statusMessageSet[413] = "Payload Too Large";
	m_statusMessageSet[414] = "URI Too Long";
	m_statusMessageSet[415] = "Unsupported Media Type";
	m_statusMessageSet[500] = "Internal Server Error";
	m_statusMessageSet[501] = "Not Implemented";
	m_statusMessageSet[502] = "Bad Gateway";
	m_statusMessageSet[503] = "Service Unavailable";
	m_statusMessageSet[504] = "Gateway Timeout";
	m_statusMessageSet[505] = "HTTP Version Not Supported";
}

Config::Config(const Config& rhs)
{
	m_configMultiMap = rhs.m_configMultiMap;
	m_mimeSet = rhs.m_mimeSet;
}

Config& Config::operator=(const Config& rhs)
{
	if (this == &rhs)
		return *this;
	m_configMultiMap = rhs.m_configMultiMap;
	m_mimeSet = rhs.m_mimeSet;
	return *this;
}

Config::~Config()
{
}

void Config::ParseConfig(std::string path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		throw std::runtime_error("Error: cannot open config file");
	}
	std::ostringstream fileStream;
	fileStream << file.rdbuf();
	std::string fileString = fileStream.str();
	fileString = ignoreComment(fileString);
	fileString = semicolonToSpaceSemicolonSpace(fileString);
	fileString = braceToSpaceBraceSpace(fileString);
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
				server.ParseServer(block);
				for (std::vector<int>::iterator it = server.GetPort().begin(); it != server.GetPort().end(); it++)
				{
					m_configMultiMap.insert(std::pair<int, Server>(*it, server));
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
	parseMime();
}


std::string Config::ignoreComment(std::string fileString)
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

std::string Config::semicolonToSpaceSemicolonSpace(std::string fileString)
{
	std::string::size_type pos = fileString.find(";");
	while (pos != std::string::npos)
	{
		fileString.replace(pos, 1, " ; ");
		pos = fileString.find(";", pos + 3);
	}
	return fileString;
}

std::string Config::braceToSpaceBraceSpace(std::string fileString)
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

Server& Config::GetServer(int port, std::string serverName)
{
	std::multimap<int, Server>::iterator it = m_configMultiMap.find(port);
	if (it == m_configMultiMap.end())
	{
		throw std::runtime_error("Error: server not found");
	}
	while (it != m_configMultiMap.end() && it->first == port)
	{
		for (std::vector<std::string>::iterator it2 = it->second.GetServerName().begin(); it2 != it->second.GetServerName().end(); it2++)
		{
			if (*it2 == serverName)
			{
				return it->second;
			}
		}
		it++;
	}
	it = m_configMultiMap.find(port);
	return it->second;
}

std::vector<int> Config::GetPorts()
{
	std::vector<int> ports;
	for (std::multimap<int, Server>::iterator it = m_configMultiMap.begin(); it != m_configMultiMap.end(); it++)
	{
		if (std::find(ports.begin(), ports.end(), it->first) == ports.end())
			ports.push_back(it->first);
	}
	return ports;
}

void Config::parseMime()
{
	std::ifstream file("./default/mime.types");
	if (!file.is_open())
	{
		throw std::runtime_error("Error: cannot open mime.types file");
	}
	std::ostringstream fileStream;
	fileStream << file.rdbuf();
	std::string fileString = fileStream.str();
	std::istringstream iss(fileString);
	std::string line;
	while (std::getline(iss, line))
	{
		std::istringstream issLine(line);
		std::string key;
		std::string value;
		issLine >> value;
		while (issLine >> key)
		{
			m_mimeSet[key] = value;
		}
		m_mimeSet.erase(key);
		key = key.substr(0, key.size() - 1);
		m_mimeSet[key] = value;
	}
}

std::string Config::GetMimeType(std::string extension)
{
	std::map<std::string, std::string>::iterator it = m_mimeSet.find(extension);
	if (it == m_mimeSet.end())
	{
		return "application/octet-stream";
	}
	return it->second;
}

std::string Config::GetStatusMessage(int status)
{
	std::map<int, std::string>::iterator it = m_statusMessageSet.find(status);
	if (it == m_statusMessageSet.end())
	{
		return "Internal Server Error";
	}
	return it->second;
}

std::string Config::GetExtension(std::string path)
{
	std::string extension;
	size_t pos_point = path.find_last_of(".");
	size_t pos_slash = path.find_last_of("/");
	if (pos_point == std::string::npos || pos_slash > pos_point)
	{
		return "";
	}
	extension = path.substr(pos_point + 1);
	return extension;
}