#include "Config.hpp"

Config::Config()
{
	mStatusMessageSet[100] = "Continue";
	mStatusMessageSet[101] = "Switching Protocols";
	mStatusMessageSet[200] = "OK";
	mStatusMessageSet[201] = "Created";
	mStatusMessageSet[202] = "Accepted";
	mStatusMessageSet[204] = "No Content";
	mStatusMessageSet[206] = "Partial Content";
	mStatusMessageSet[300] = "Multiple Choices";
	mStatusMessageSet[301] = "Moved Permanently";
	mStatusMessageSet[302] = "Found";
	mStatusMessageSet[303] = "See Other";
	mStatusMessageSet[304] = "Not Modified";
	mStatusMessageSet[307] = "Temporary Redirect";
	mStatusMessageSet[308] = "Permanent Redirect";
	mStatusMessageSet[400] = "Bad Request";
	mStatusMessageSet[401] = "Unauthorized";
	mStatusMessageSet[403] = "Forbidden";
	mStatusMessageSet[404] = "Not Found";
	mStatusMessageSet[405] = "Method Not Allowed";
	mStatusMessageSet[406] = "Not Acceptable";
	mStatusMessageSet[408] = "Request Timeout";
	mStatusMessageSet[409] = "Conflict";
	mStatusMessageSet[411] = "Length Required";
	mStatusMessageSet[413] = "Payload Too Large";
	mStatusMessageSet[414] = "URI Too Long";
	mStatusMessageSet[415] = "Unsupported Media Type";
	mStatusMessageSet[431] = "Request Header Fields Too Large";
	mStatusMessageSet[500] = "Internal Server Error";
	mStatusMessageSet[501] = "Not Implemented";
	mStatusMessageSet[502] = "Bad Gateway";
	mStatusMessageSet[503] = "Service Unavailable";
	mStatusMessageSet[504] = "Gateway Timeout";
	mStatusMessageSet[505] = "HTTP Version Not Supported";
}

Config::Config(const Config& rhs)
{
	mServer = rhs.mServer;
	mMimeSet = rhs.mMimeSet;
	mStatusMessageSet = rhs.mStatusMessageSet;
}

Config& Config::operator=(const Config& rhs)
{
	if (this == &rhs)
		return *this;
	mServer = rhs.mServer;
	mMimeSet = rhs.mMimeSet;
	mStatusMessageSet = rhs.mStatusMessageSet;
	return *this;
}

Config::~Config()
{
}

void Config::PrintConfig()
{
	std::cout << "------------config------------" << std::endl;
	for (std::multimap<int, Server>::iterator it = mServer.begin(); it != mServer.end(); it++)
	{
		std::cout << "----------------------------\n";
		std::cout << "----------------------------\n";
		std::cout << "port: " << it->first << std::endl;
		it->second.PrintServer();
		std::cout << "----------------------------\n";
		std::cout << "----------------------------\n";
	}
}

void Config::ParseConfig(std::string path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		throw std::runtime_error("Error: cannot open config file");
	}
	std::ostringstream fileStream;
	fileStream << file.rdbuf();
	std::string fileString = fileStream.str();
	fileString = setConfigString(fileString);
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
			if (issLine >> token)
			{
				throw std::runtime_error("Error: config file is invalid\nline: " + line + "\n");
			}
			std::getline(iss, line);
			issLine.clear();
			issLine.str(line);
			if (issLine >> token && token == "{")
			{
				if (issLine >> token)
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
				std::vector<int> &ports = server.GetPort();
				for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); it++)
				{
					if (mServer.find(*it) == mServer.end())
						mServer.insert(std::pair<int, Server>(*it, server));
					else
						throw std::runtime_error("Error: config file is invalid\nport is duplicated\n");
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
	// std::cout << "mServer.size(): " << mServer.size() << "\n";
	if (mServer.size() == 0)
		throw std::runtime_error("Error: config file is invalid\n");
}

std::string Config::setConfigString(std::string fileString)
{
	std::string::size_type pos = fileString.find("#");
	while (pos != std::string::npos)
	{
		std::string::size_type end = fileString.find("\n", pos);
		fileString.erase(pos, end - pos);
		pos = fileString.find("#");
	}
	fileString = addSpaceAroundChar(fileString, ';');
	fileString = addSpaceAroundChar(fileString, '{');
	fileString = addSpaceAroundChar(fileString, '}');
	return fileString;
}

std::string Config::addSpaceAroundChar(std::string fileString, char c)
{
	std::string::size_type pos = fileString.find(c);
	std::string replace;
	replace += c;
	replace += " ";
	replace = " " + replace;
	while (pos != std::string::npos)
	{
		fileString.replace(pos, 1, replace);
		pos = fileString.find(c, pos + 3);
	}
	return fileString;
}

std::vector<int> Config::GetPorts()
{
	std::vector<int> ports;
	for (std::multimap<int, Server>::iterator it = mServer.begin(); it != mServer.end(); it++)
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
			mMimeSet[key] = value;
		}
		mMimeSet.erase(key);
		key = key.substr(0, key.size() - 1);
		mMimeSet[key] = value;
	}
}

std::string Config::GetMimeType(std::string extension)
{
	std::map<std::string, std::string>::iterator it = mMimeSet.find(extension);
	if (it == mMimeSet.end())
	{
		return "application/octet-stream";
	}
	return it->second;
}

std::string Config::GetStatusMessage(int status)
{
	std::map<int, std::string>::iterator it = mStatusMessageSet.find(status);
	if (it == mStatusMessageSet.end())
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

Server& Config::getServer(int port, std::string serverName)
{
	std::multimap<int, Server>::iterator it = mServer.find(port);
	if (it == mServer.end())
	{
		return mServer.begin()->second;
	}
	while (it != mServer.end() && it->first == port)
	{
		std::vector<std::string> &serverNames = it->second.GetServerName();
		if (std::find(serverNames.begin(), serverNames.end(), serverName) != serverNames.end())
		{
			return it->second;
		}
		it++;
	}
	it = mServer.find(port);
	return it->second;
}

std::string& Config::GetRoot(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	if (location.GetRootFlag() == false)
		return server.GetRoot();
	else
		return location.GetRoot();
}

std::string Config::GetFilePath(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	std::string originPath = path;
	if (location.GetCgiPathFlag() == true)
	{
		size_t pos = path.find_last_of("/");
		path = path.substr(0, pos);
		Location &tmpLocation = server.GetLocation(path);
		std::string lpath = tmpLocation.GetPath();
		std::string root = GetRoot(port, serverName, path);

		std::string tmp = originPath.substr(lpath.length());
		if (tmp[0] != '/')
			tmp = "/" + tmp;
		return root + tmp;
	}
	std::string lpath = location.GetPath();
	std::string root = GetRoot(port, serverName, path);
	std::string tmp = originPath.substr(lpath.length());
	if (tmp[0] != '/')
		tmp = "/" + tmp;
	return root + tmp;
}

std::vector<std::string>& Config::GetMethods(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	if (location.GetMethodFlag() == false)
		return server.GetMethods();
	else
		return location.GetMethods();
}

std::string Config::GetCgiPath(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	if (location.GetCgiPathFlag() == false)
		return "";
	else
		return location.GetCgiPath();
}

std::string Config::GetRedirectionPath(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	if (location.GetRedirectionFlag() == false)
		return "";
	else
		return location.GetRedirectionPath();
}

std::string Config::makeErrorPage(int status, const std::string statusMessage) 
{
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"en\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"UTF-8\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <title>" << status << " " << statusMessage << "</title>\n";
    html << "    <style>\n";
    html << "        body {\n";
    html << "            font-family: Arial, sans-serif;\n";
    html << "            text-align: center;\n";
    html << "            padding: 50px;\n";
    html << "        }\n";
    html << "        h1 {\n";
    html << "            color: #ff6347;\n";
    html << "            font-size: 48px;\n";
    html << "        }\n";
    html << "        p {\n";
    html << "            color: #666;\n";
    html << "            font-size: 24px;\n";
    html << "        }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <h1>" << status << " " << statusMessage << "</h1>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string Config::GetErrorPage(int port, std::string serverName, int status)
{
	Server& server = getServer(port, serverName);
	std::map<int, std::string>& errorPages = server.GetErrorPages();
	std::map<int, std::string>::iterator it = errorPages.find(status);
	if (it == errorPages.end())
	{
		std::fstream file;
		return makeErrorPage(status, GetStatusMessage(status));
	}
	std::ifstream file(it->second.c_str());
	if (!file.is_open())
	{
		std::fstream file;
		return makeErrorPage(status, GetStatusMessage(status));
	}
	std::ostringstream fileStream;
	fileStream << file.rdbuf();
	std::string fileString = fileStream.str();
	return fileString;
}

int Config::GetLimitBodySize(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	
	if (location.GetLimitBodySizeFlag() == false)
	{
		if (server.GetLimitBodySizeFlag() == false)
			return -1;
		else
			return server.GetLimitBodySize();
	}
	else
		return location.GetLimitBodySize();
}

bool Config::IsAutoIndex(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	return location.GetAutoIndex();
}

std::vector<std::string>& Config::GetIndexes(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	if (location.GetIndexFlag() == false)
		return server.GetIndexes();
	else
		return location.GetIndexes();
}

int Config::GetRedirectStatus(int port, std::string serverName, std::string path)
{
	Server& server = getServer(port, serverName);
	Location& location = server.GetLocation(path);
	if (location.GetRedirectionFlag() == false)
		return 0;
	else
		return location.GetRedirectionCode();
}