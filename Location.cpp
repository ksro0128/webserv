#include "Location.hpp"

Location::Location()
{
	_path = "/";
	_root = "";
	_index.clear();
	_method.clear();
	_autoIndex = false;
	_redirectionCode = 0;
	_redirectionUri = "";
	_rootFlag = false;
	_indexFlag = false;
	_methodFlag = false;
	_autoIndexFlag = false;
	_redirectionFlag = false;
}

Location::Location(const Location& rhs)
{
	*this = rhs;
}

Location& Location::operator=(const Location& rhs)
{
	if (this == &rhs)
		return *this;
	_path = rhs._path;
	_root = rhs._root;
	_index = rhs._index;
	_method = rhs._method;
	_autoIndex = rhs._autoIndex;
	_redirectionCode = rhs._redirectionCode;
	_redirectionUri = rhs._redirectionUri;
	_rootFlag = rhs._rootFlag;
	_indexFlag = rhs._indexFlag;
	_methodFlag = rhs._methodFlag;
	_autoIndexFlag = rhs._autoIndexFlag;
	_redirectionFlag = rhs._redirectionFlag;

	return *this;
}

Location::~Location()
{
}

void Location::printLocation()
{
	std::cout << "path: " << _path << std::endl;
	std::cout << "root: " << _root << std::endl;
	std::cout << "index: ";
	for (size_t i = 0; i < _index.size(); i++)
		std::cout << _index[i] << " ";
	std::cout << std::endl;
	std::cout << "method: ";
	for (size_t i = 0; i < _method.size(); i++)
		std::cout << _method[i] << " ";
	std::cout << std::endl;
	std::cout << "autoindex: ";
	if (_autoIndex)
		std::cout << "on" << std::endl;
	else
		std::cout << "off" << std::endl;
	std::cout << "redirectionCode: " << _redirectionCode << std::endl;
	std::cout << "redirectionUri: " << _redirectionUri << std::endl;

}

void Location::parseLocation(std::string block, std::string path)
{
	std::istringstream issBlock(block);
	std::string line;
	while (std::getline(issBlock, line))
	{
		std::istringstream issLine(line);
		std::string token;
		issLine >> token;
		if (token == "root")
		{
			try
			{
				_parseRoot(issLine);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "index")
		{
			try
			{
				_parseIndex(issLine);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "method")
		{
			try
			{
				_parseMethod(issLine);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "return")
		{
			try
			{
				_parseRedirection(issLine);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "autoindex")
		{
			try
			{
				_parseAutoIndex(issLine);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else
			throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
	}
	_path = path;
}

void Location::_parseRoot(std::istringstream& issLine)
{
	if (_rootFlag)
		throw std::runtime_error("root is already defined");
	_rootFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("root is not defined");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.length(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("root is invalid");
	}
	_root = token;
	if (!(issLine >> token))
		throw std::runtime_error("root is invalid");
	if (token != ";")
		throw std::runtime_error("root is invalid");
	if (issLine >> token)
		throw std::runtime_error("root is invalid");
}

void Location::_parseIndex(std::istringstream& issLine)
{
	if (_indexFlag)
		throw std::runtime_error("index is already defined");
	_indexFlag = true;
	std::string token;
	while (issLine >> token)
	{
		if (token == ";")
			break;
		std::string reservedChars = ":/?#[]@!$&'()*+,;=";
		std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
		for (size_t i = 0; i < token.length(); i++)
		{
			if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
				throw std::runtime_error("index is invalid");
		}
		_index.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("index is invalid");
	if (issLine >> token)
		throw std::runtime_error("index is invalid");
}

void Location::_parseMethod(std::istringstream& issLine)
{
	if (_methodFlag)
		throw std::runtime_error("method is already defined");
	_methodFlag = true;
	std::string token;
	std::vector<std::string> invalidMethods;
	invalidMethods.push_back("GET");
	invalidMethods.push_back("POST");
	invalidMethods.push_back("DELETE");
	invalidMethods.push_back("HEAD");
	invalidMethods.push_back("PUT");
	while (issLine >> token)
	{
		if (token == ";")
			break;
		if (std::find(invalidMethods.begin(), invalidMethods.end(), token) == invalidMethods.end())
			throw std::runtime_error("method is invalid");
		_method.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("method is invalid");
	if (issLine >> token)
		throw std::runtime_error("method is invalid");
}

void Location::_parseAutoIndex(std::istringstream& issLine)
{
	if (_autoIndexFlag)
		throw std::runtime_error("autoindex is already defined");
	_autoIndexFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("autoindex is not defined");
	if (token == "on")
		_autoIndex = true;
	else if (token == "off")
		_autoIndex = false;
	else
		throw std::runtime_error("autoindex is invalid");
	if (!(issLine >> token))
		throw std::runtime_error("autoindex is invalid");
	if (token != ";")
		throw std::runtime_error("autoindex is invalid");
	if (issLine >> token)
		throw std::runtime_error("autoindex is invalid");
}

void Location::_parseRedirection(std::istringstream& issLine)
{
	if (_redirectionFlag)
		throw std::runtime_error("return is already defined");
	_redirectionFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("return is not defined");
	if (token != "301" && token != "302" && token != "303" && token != "307" && token != "308")
		throw std::runtime_error("return is invalid");
	_redirectionCode = strtol(token.c_str(), NULL, 10);
	if (!(issLine >> token))
		throw std::runtime_error("return is invalid");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.length(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("return is invalid");
	}
	_redirectionUri = token;
	if (!(issLine >> token))
		throw std::runtime_error("return is invalid");
	if (token != ";")
		throw std::runtime_error("return is invalid");
	if (issLine >> token)
		throw std::runtime_error("return is invalid");
}

void Location::setLocation(std::string root, std::vector<std::string> index, std::vector<std::string> method)
{
	if (!_rootFlag)
		_root = root;
	if (!_indexFlag)
		_index = index;
	if (!_methodFlag)
		_method = method;
}

std::string& Location::getPath()
{
	return _path;
}

std::string& Location::getRoot()
{
	return _root;
}

std::vector<std::string>& Location::getIndex()
{
	return _index;
}

std::vector<std::string>& Location::getMethod()
{
	return _method;
}


bool& Location::getAutoIndex()
{
	return _autoIndex;
}

int& Location::getRedirectionCode()
{
	return _redirectionCode;
}

std::string& Location::getRedirectionUri()
{
	return _redirectionUri;
}

bool& Location::getRedicetionFlag()
{
	return _redirectionFlag;
}
