#include "Server.hpp"

Server::Server() // 내일 오전 server구현 location 구현 config에서 host port path받으면 정보 반환 구현
{
	mPort.push_back(80);
	mServerNames.push_back("localhost");
	mRoot = "./";
	mLimitBodySize = -1;
	mMethods.push_back("GET");
	mLocations.clear();
	mCgi.clear();

	mPortFlag = false;
	mServerNameFlag = false;
	mRootFlag = false;
	mIndexFlag = false;
	mMethodFlag = false;
	mLimitBodySizeFlag = false;
	mLocationFlag = false;
	mCgiFlag = false;
}

Server::Server(const Server& rhs)
{
	mPort = rhs.mPort;
	mServerNames = rhs.mServerNames;
	mRoot = rhs.mRoot;
	mIndexes = rhs.mIndexes;
	mMethods = rhs.mMethods;
	mErrorPages = rhs.mErrorPages;
	mLimitBodySize = rhs.mLimitBodySize;
	mLocations = rhs.mLocations;
	mCgi = rhs.mCgi;

	mPortFlag = rhs.mPortFlag;
	mServerNameFlag = rhs.mServerNameFlag;
	mRootFlag = rhs.mRootFlag;
	mIndexFlag = rhs.mIndexFlag;
	mMethodFlag = rhs.mMethodFlag;
	mLimitBodySizeFlag = rhs.mLimitBodySizeFlag;
	mLocationFlag = rhs.mLocationFlag;
	mCgiFlag = rhs.mCgiFlag;
}

Server& Server::operator=(const Server& rhs)
{
	if (this == &rhs)
		return *this;
	mPort = rhs.mPort;
	mServerNames = rhs.mServerNames;
	mRoot = rhs.mRoot;
	mIndexes = rhs.mIndexes;
	mMethods = rhs.mMethods;
	mErrorPages = rhs.mErrorPages;
	mLimitBodySize = rhs.mLimitBodySize;
	mLocations = rhs.mLocations;
	mCgi = rhs.mCgi;

	mPortFlag = rhs.mPortFlag;
	mServerNameFlag = rhs.mServerNameFlag;
	mRootFlag = rhs.mRootFlag;
	mIndexFlag = rhs.mIndexFlag;
	mMethodFlag = rhs.mMethodFlag;
	mLimitBodySizeFlag = rhs.mLimitBodySizeFlag;
	mLocationFlag = rhs.mLocationFlag;
	mCgiFlag = rhs.mCgiFlag;
	return *this;
}

Server::~Server()
{
}

void Server::PrintServer()
{
	std::cout << "port: ";
	for (size_t i = 0; i < mPort.size(); i++)
		std::cout << mPort[i] << " ";
	std::cout << std::endl;
	std::cout << "server_name: ";
	for (size_t i = 0; i < mServerNames.size(); i++)
		std::cout << mServerNames[i] << " ";
	std::cout << std::endl;
	std::cout << "root: " << mRoot << std::endl;
	std::cout << "index: ";
	for (size_t i = 0; i < mIndexes.size(); i++)
		std::cout << mIndexes[i] << " ";
	std::cout << std::endl;
	std::cout << "method: ";
	for (size_t i = 0; i < mMethods.size(); i++)
		std::cout << mMethods[i] << " ";
	std::cout << std::endl;
	std::cout << "error_page: ";
	for (std::map<int, std::string>::iterator it = mErrorPages.begin(); it != mErrorPages.end(); it++)
		std::cout << it->first << " " << it->second << " ";
	std::cout << std::endl;
	std::cout << "limit_body_size: " << mLimitBodySize << std::endl;
	std::cout << "*locations*" << std::endl;
	for (size_t i = 0; i < mLocations.size(); i++)
		mLocations[i].PrintLocation();
	std::cout << "*cgi*" << std::endl;
	for (size_t i = 0; i < mCgi.size(); i++)
		mCgi[i].PrintLocation();
}

void Server::ParseServer(std::string block)
{
	std::istringstream iss(block);
	std::string line;
	while (std::getline(iss, line))
	{
		std::istringstream issLine(line);
		std::string token;
		issLine >> token;
		if (token.size() == 0)
			continue;
		if (token == "listen")
		{
			parsePort(issLine);
		}
		else if (token == "server_name")
		{
			parseServerName(issLine);
		}
		else if (token == "root")
		{
			parseRoot(issLine);
		}
		else if (token == "index")
		{
			parseIndex(issLine);
		}
		else if (token == "method")
		{
			parseMethod(issLine);
		}
		else if (token == "error_page")
		{
			parseErrorPage(issLine);
		}
		else if (token == "limit_body_size")
		{
			parseLimitBodySize(issLine);
		}
		else if (token == "location")
		{
			parseLocation(iss, issLine);
		}
		else
		{
			throw std::runtime_error("Error: config file is invalid\nline: " + line + "\n");
		}
	}
	// if (!mLocationFlag)
	// {
	// 	Location location;
	// 	location.ParseLocation("", "/");
	// 	mLocations.push_back(location);
	// }
	bool flag = false;
	for (size_t i = 0; i < mLocations.size(); i++)
	{
		if (mLocations[i].GetPath() == "/")
		{
			flag = true;
			break;
		}
	}
	if (!flag)
	{
		Location location;
		location.ParseLocation("", "/");
		mLocations.push_back(location);
	}
	if (!mCgiFlag)
	{
		Location location;
		location.ParseLocation("", "");
		mCgi.push_back(location);
	}
}

void Server::parsePort(std::istringstream& issLine)
{
	if (mPortFlag)
		throw std::runtime_error("Error: port is already defined\n");
	mPortFlag = true;
	mPort.clear();
	std::string token;
	while (issLine >> token && token != ";")
	{
		for (size_t i = 0; i < token.size(); i++)
		{
			if (!std::isdigit(token[i]))
				throw std::runtime_error("Error: port is invalid\n");
		}
		long port = strtol(token.c_str(), NULL, 10);
		if (port < 0 || port > 65535)
			throw std::runtime_error("Error: port is invalid\n");
		mPort.push_back(port);
	}
	if (token != ";")
		throw std::runtime_error("Error: port is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("Error: port is invalid\n");
	if (mPort.size() == 0)
		throw std::runtime_error("Error: port is invalid\n");
}

void Server::parseServerName(std::istringstream& issLine)
{
	if (mServerNameFlag)
		throw std::runtime_error("Error: server_name is already defined\n");
	mServerNameFlag = true;
	mServerNames.clear();
	std::string token;
	while (issLine >> token && token != ";")
	{
		mServerNames.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("Error: server_name is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("Error: server_name is invalid\n");
	if (mServerNames.size() == 0)
		throw std::runtime_error("Error: server_name is invalid\n");
}

void Server::parseRoot(std::istringstream& issLine)
{
	if (mRootFlag)
		throw std::runtime_error("Error: root is already defined\n");
	mRootFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("Error: root is not defined\n");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.length(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("Error: root is invalid\n");
	}
	mRoot = token;
	if (!(issLine >> token))
		throw std::runtime_error("Error: root is invalid\n");
	if (token != ";")
		throw std::runtime_error("Error: root is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("Error: root is invalid\n");
}

void Server::parseIndex(std::istringstream& issLine)
{
	if (mIndexFlag)
		throw std::runtime_error("Error: index is already defined\n");
	mIndexFlag = true;
	mIndexes.clear();
	std::string token;
	while (issLine >> token && token != ";")
	{
		std::string reservedChars = ":/?#[]@!$&'()*+,;=";
		std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
		for (size_t i = 0; i < token.length(); i++)
		{
			if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
				throw std::runtime_error("Error: index is invalid\n");
		}
		mIndexes.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("Error: index is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("Error: index is invalid\n");
	if (mIndexes.size() == 0)
		throw std::runtime_error("Error: index is invalid\n");
}

void Server::parseMethod(std::istringstream& issLine)
{
	if (mMethodFlag)
		throw std::runtime_error("Error: method is already defined\n");
	mMethodFlag = true;
	mMethods.clear();
	std::string token;
	std::vector<std::string> validMethods;
	validMethods.push_back("GET");
	validMethods.push_back("HEAD");
	validMethods.push_back("POST");
	validMethods.push_back("PUT");
	validMethods.push_back("DELETE");
	while (issLine >> token && token != ";")
	{
		if (std::find(validMethods.begin(), validMethods.end(), token) == validMethods.end())
			throw std::runtime_error("method directive is invalid\n");
		mMethods.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("Error: method is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("Error: method is invalid\n");
	if (mMethods.size() == 0)
		throw std::runtime_error("Error: method is invalid\n");
}

void Server::parseErrorPage(std::istringstream& issLine)
{
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("Error: error_page is not defined\n");
	long errorCode = strtol(token.c_str(), NULL, 10);
	if (errorCode < 100 || errorCode > 599)
		throw std::runtime_error("Error: error_page is invalid\n");
	if (!(issLine >> token))
		throw std::runtime_error("Error: error_page is invalid\n");
	std::ifstream file(token.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: error_page is invalid\n");
	file.close();
	mErrorPages[errorCode] = token;
	if (!(issLine >> token))
		throw std::runtime_error("Error: error_page is invalid\n");
	if (token != ";")
		throw std::runtime_error("Error: error_page is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("Error: error_page is invalid\n");
}

void Server::parseLimitBodySize(std::istringstream& issLine)
{
	if (mLimitBodySizeFlag)
		throw std::runtime_error("Error: limit_body_size is already defined\n");
	mLimitBodySizeFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("Error: limit_body_size is not defined\n");
	for (size_t i = 0; i < token.size(); i++)
	{
		if (!std::isdigit(token[i]))
			throw std::runtime_error("Error: limit_body_size is invalid\n");
	}
	mLimitBodySize = strtol(token.c_str(), NULL, 10);
	if (mLimitBodySize < 0)
		throw std::runtime_error("Error: limit_body_size is invalid\n");
	if (!(issLine >> token))
		throw std::runtime_error("Error: limit_body_size is invalid\n");
	if (token != ";")
		throw std::runtime_error("Error: limit_body_size is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("Error: limit_body_size is invalid\n");
}

void Server::parseLocation(std::istringstream& iss, std::istringstream& issLine)
{
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("Error: location path is not defined\n");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.length(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("Error: location path is invalid\n");
	}
	std::string path = token;
	if (path[0] != '/' && path.find("*.") != 0)
		throw std::runtime_error("Error: location path is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("Error: location is invalid\n");
	std::string line;
	std::string block;
	if (std::getline(iss, line))
	{
		issLine.clear();
		issLine.str(line);
		issLine >> token;
		if (token != "{")
			throw std::runtime_error("Error: location block is invalid\n");
		if (issLine >> token)
			throw std::runtime_error("Error: location block is invalid\n");
	}
	while (std::getline(iss, line))
	{
		if (line.find("}") != std::string::npos)
			break;
		block += line + "\n";
	}
	Location location;
	location.ParseLocation(block, path);
	if (path[0] == '/')
	{
		mLocationFlag = true;
		mLocations.push_back(location);
	}
	else
	{
		mCgiFlag = true;
		mCgi.push_back(location);
	}
}

std::vector<int>& Server::GetPort() { return mPort; }
std::vector<std::string>& Server::GetServerName() { return mServerNames; }
std::string& Server::GetRoot() { return mRoot; }
std::vector<std::string>& Server::GetIndexes() { return mIndexes; }
std::vector<std::string>& Server::GetMethods() { return mMethods; }
std::map<int, std::string>& Server::GetErrorPages() { return mErrorPages; }
int& Server::GetLimitBodySize() { return mLimitBodySize; }
std::vector<Location>& Server::GetLocations() { return mLocations; }
std::vector<Location>& Server::GetCgi() { return mCgi; }
bool Server::GetPortFlag() { return mPortFlag; }
bool Server::GetServerNameFlag() { return mServerNameFlag; }
bool Server::GetRootFlag() { return mRootFlag; }
bool Server::GetIndexFlag() { return mIndexFlag; }
bool Server::GetMethodFlag() { return mMethodFlag; }
bool Server::GetLimitBodySizeFlag() { return mLimitBodySizeFlag; }
bool Server::GetLocationFlag() { return mLocationFlag; }
bool Server::GetCgiFlag() { return mCgiFlag; }

std::string Server::getExtension(std::string path)
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



Location& Server::GetLocation(std::string path)
{
	std::string extension = getExtension(path);
	if (IsCgi(extension))
		return GetCgiLocation(extension);
	int idx = -1;
	for (size_t i = 0; i < mLocations.size(); i++)
	{
		std::string &lpath = mLocations[i].GetPath();
		int pos = path.find(lpath);
		if (pos == 0 && (path[lpath.size()] == '/' || path[lpath.size()] == '\0'))
		{
			if (idx == -1)
				idx = i;
			else if (mLocations[i].GetPath().size() > mLocations[idx].GetPath().size())
				idx = i;
		}
	}
	if (idx == -1)
		idx = 0;
	return mLocations[idx];
}

bool Server::IsCgi(std::string extension)
{
	if (mCgiFlag == false)
		return false;
	for (size_t i = 0; i < mCgi.size(); i++)
	{
		std::string &cgiPath = mCgi[i].GetPath();
		if (extension == cgiPath)
			return true;
	}
	return false;
}

Location& Server::GetCgiLocation(std::string extension)
{
	for (size_t i = 0; i < mCgi.size(); i++)
	{
		std::string &cgiPath = mCgi[i].GetPath();
		if (extension == cgiPath)
			return mCgi[i];
	}
	return mCgi[0];
}