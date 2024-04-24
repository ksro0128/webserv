#include "Location.hpp"

Location::Location()
{
	mPath = "";
	mRoot = "";
	mIndexes.clear();
	mMethods.clear();
	mRedirectionPath = "";
	mRedirectionCode = 0;
	mAutoIndex = false;
	mCgiPath = "";
	mLimitBodySize = -1;

	mRootFlag = false;
	mIndexFlag = false;
	mMethodFlag = false;
	mAutoIndexFlag = false;
	mRedirectionFlag = false;
	mCgiPathFlag = false;
	mLimitBodySizeFlag = false;

}

Location::Location(const Location& rhs)
{
	mPath = rhs.mPath;
	mRoot = rhs.mRoot;
	mIndexes = rhs.mIndexes;
	mMethods = rhs.mMethods;
	mAutoIndex = rhs.mAutoIndex;
	mRedirectionCode = rhs.mRedirectionCode;
	mRedirectionPath = rhs.mRedirectionPath;
	mCgiPath = rhs.mCgiPath;
	mLimitBodySize = rhs.mLimitBodySize;

	mRootFlag = rhs.mRootFlag;
	mIndexFlag = rhs.mIndexFlag;
	mMethodFlag = rhs.mMethodFlag;
	mAutoIndexFlag = rhs.mAutoIndexFlag;
	mRedirectionFlag = rhs.mRedirectionFlag;
	mCgiPathFlag = rhs.mCgiPathFlag;
	mLimitBodySizeFlag = rhs.mLimitBodySizeFlag;
}

Location& Location::operator=(const Location& rhs)
{
	if (this == &rhs)
		return *this;
	mPath = rhs.mPath;
	mRoot = rhs.mRoot;
	mIndexes = rhs.mIndexes;
	mMethods = rhs.mMethods;
	mAutoIndex = rhs.mAutoIndex;
	mRedirectionCode = rhs.mRedirectionCode;
	mRedirectionPath = rhs.mRedirectionPath;
	mCgiPath = rhs.mCgiPath;
	mLimitBodySize = rhs.mLimitBodySize;

	mRootFlag = rhs.mRootFlag;
	mIndexFlag = rhs.mIndexFlag;
	mMethodFlag = rhs.mMethodFlag;
	mAutoIndexFlag = rhs.mAutoIndexFlag;
	mRedirectionFlag = rhs.mRedirectionFlag;
	mCgiPathFlag = rhs.mCgiPathFlag;
	mLimitBodySizeFlag = rhs.mLimitBodySizeFlag;
	return *this;
}

Location::~Location()
{
}

void Location::PrintLocation()
{
	std::cout << "------------location------------" << std::endl;
	std::cout << "Location path: " << mPath << std::endl;
	std::cout << "Root: " << mRoot << std::endl;
	std::cout << "Indexes: ";
	for (size_t i = 0; i < mIndexes.size(); i++)
		std::cout << mIndexes[i] << " ";
	std::cout << std::endl;
	std::cout << "Methods: ";
	for (size_t i = 0; i < mMethods.size(); i++)
		std::cout << mMethods[i] << " ";
	std::cout << std::endl;
	std::cout << "Autoindex: " << mAutoIndex << std::endl;
	std::cout << "Redirection code: " << mRedirectionCode << std::endl;
	std::cout << "Redirection path: " << mRedirectionPath << std::endl;
	std::cout << "Cgi path: " << mCgiPath << std::endl;
}

void Location::ParseLocation(std::string block, std::string path)
{
	mPath = path;
	std::istringstream iss(block);
	std::string line;
	while (std::getline(iss, line))
	{
		std::istringstream issLine(line);
		std::string token;
		issLine >> token;
		if (token.size() == 0)
			continue;
		if (token == "root")
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
		else if (token == "autoindex")
		{
			parseAutoIndex(issLine);
		}
		else if (token == "return")
		{
			parseRedirection(issLine);
		}
		else if (token == "cgi_path")
		{
			parseCgiPath(issLine);
		}
		else if (token == "limit_body_size")
		{
			parseLimitBodySize(issLine);
		}
		else
			throw std::runtime_error("location block is invalid");
	}
	if (mPath.find("*.") == 0)
		mPath = mPath.substr(2);
	else if (mPath[mPath.size() - 1] == '/')
		mPath = mPath.substr(0, mPath.size() - 1);
}

void Location::parseRoot(std::istringstream& issLine)
{
	if (mRootFlag)
		throw std::runtime_error("root is already defined");
	mRootFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("root is invalid");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.size(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("root is invalid");
	}
	mRoot = token;
	if (!(issLine >> token))
		throw std::runtime_error("root is invalid");
	if (token != ";")
		throw std::runtime_error("root is invalid");
	if (issLine >> token)
		throw std::runtime_error("root is invalid");
}

void Location::parseIndex(std::istringstream& issLine)
{
	if (mIndexFlag)
		throw std::runtime_error("index is already defined");
	mIndexFlag = true;
	std::string token;
	while (issLine >> token)
	{
		if (token == ";")
			break;
		std::string reservedChars = ":/?#[]@!$&'()*+,;=";
		std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
		for (size_t i = 0; i < token.size(); i++)
		{
			if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
				throw std::runtime_error("index is invalid");
		}
		mIndexes.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("index is invalid");
	if (issLine >> token)
		throw std::runtime_error("index is invalid");
}

void Location::parseMethod(std::istringstream& issLine)
{
	if (mMethodFlag)
		throw std::runtime_error("method is already defined");
	mMethodFlag = true;
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
			throw std::runtime_error("method is invalid");
		mMethods.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("method is invalid");
	if (issLine >> token)
		throw std::runtime_error("method is invalid");
	if (mMethods.empty())
		throw std::runtime_error("method is invalid");
}

void Location::parseAutoIndex(std::istringstream& issLine)
{
	if (mAutoIndexFlag)
		throw std::runtime_error("autoindex is already defined");
	mAutoIndexFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("autoindex is invalid");
	if (token == "on")
		mAutoIndex = true;
	else if (token == "off")
		mAutoIndex = false;
	else
		throw std::runtime_error("autoindex is invalid");
	if (!(issLine >> token))
		throw std::runtime_error("autoindex is invalid");
	if (token != ";")
		throw std::runtime_error("autoindex is invalid");
	if (issLine >> token)
		throw std::runtime_error("autoindex is invalid");
}

void Location::parseRedirection(std::istringstream& issLine)
{
	if (mRedirectionFlag)
		throw std::runtime_error("redirection is already defined");
	mRedirectionFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("redirection is invalid");
	if (token != "301" && token != "302" && token != "303" && token != "307" && token != "308")
		throw std::runtime_error("redirection is invalid");
	mRedirectionCode = strtol(token.c_str(), NULL, 10);
	if (!(issLine >> token))
		throw std::runtime_error("redirection is invalid");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.size(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("redirection is invalid");
	}
	mRedirectionPath = token;
	if (!(issLine >> token))
		throw std::runtime_error("redirection is invalid");
	if (token != ";")
		throw std::runtime_error("redirection is invalid");
	if (issLine >> token)
		throw std::runtime_error("redirection is invalid");
}

void Location::parseCgiPath(std::istringstream& issLine)
{
	if (mCgiPathFlag)
		throw std::runtime_error("cgi_path is already defined");
	mCgiPathFlag = true;
	if (mPath.find("*.") == std::string::npos)
		throw std::runtime_error("cgi_path is invalid");
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("cgi_path is invalid");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.size(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("cgi_path is invalid");
	}
	struct stat fileinfo;
	if (stat(token.c_str(), &fileinfo) != 0)
		throw std::runtime_error("cgi_path is invalid");
	if (!(fileinfo.st_mode & S_IXUSR))
		throw std::runtime_error("cgi_path is invalid");
	mCgiPath = token;
	if (!(issLine >> token))
		throw std::runtime_error("cgi_path is invalid");
	if (token != ";")
		throw std::runtime_error("cgi_path is invalid");
	if (issLine >> token)
		throw std::runtime_error("cgi_path is invalid");
}

void Location::parseLimitBodySize(std::istringstream& issLine)
{
	if (mLimitBodySizeFlag)
		throw std::runtime_error("limit_body_size is already defined");
	mLimitBodySizeFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("limit_body_size is invalid");
	for (size_t i = 0; i < token.size(); i++)
	{
		if (!isdigit(token[i]))
			throw std::runtime_error("limit_body_size is invalid");
	}
	mLimitBodySize = strtol(token.c_str(), NULL, 10);
	if (!(issLine >> token))
		throw std::runtime_error("limit_body_size is invalid");
	if (token != ";")
		throw std::runtime_error("limit_body_size is invalid");
	if (issLine >> token)
		throw std::runtime_error("limit_body_size is invalid");
}


std::string& Location::GetPath() { return mPath; }
std::string& Location::GetRoot() { return mRoot; }
std::vector<std::string>& Location::GetIndexes() { return mIndexes; }
std::vector<std::string>& Location::GetMethods() { return mMethods; }
bool& Location::GetAutoIndex() { return mAutoIndex; }
int Location::GetRedirectionCode() { return mRedirectionCode; }
std::string& Location::GetRedirectionPath() { return mRedirectionPath; }
std::string& Location::GetCgiPath() { return mCgiPath; }
int& Location::GetLimitBodySize() { return mLimitBodySize; }
bool Location::GetRootFlag() { return mRootFlag; }
bool Location::GetIndexFlag() { return mIndexFlag; }
bool Location::GetMethodFlag() { return mMethodFlag; }
bool Location::GetAutoIndexFlag() { return mAutoIndexFlag; }
bool Location::GetRedirectionFlag() { return mRedirectionFlag; }
bool Location::GetCgiPathFlag() { return mCgiPathFlag; }
bool Location::GetLimitBodySizeFlag() { return mLimitBodySizeFlag; }
