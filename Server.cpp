#include "Server.hpp"

Server::Server()
{
	_portFlag = false;
	_serverNameFlag = false;
	_rootFlag = false;
	_indexFlag = false;
	_methodFlag = false;
	_errorPageFlag = false;
	_locationFlag = false;
	_limitBodySizeFlag = false;
	_cgiFlag = false;
	_port.push_back(80);
	_serverName.push_back("localhost");
	_root = ".";
	_index.push_back("index.html");
	_method.push_back("GET");
	_method.push_back("POST");
	_method.push_back("DELETE");
	_errorPage[400] = "./errorpage/400.html";
	_location.push_back(Location());
	_limitBodySize = 1000000;
	_cgi.clear();
}

Server::Server(const Server& rhs)
{
	*this = rhs;
}

Server& Server::operator=(const Server& rhs)
{
	if (this == &rhs)
		return *this;
	_portFlag = rhs._portFlag;
	_serverNameFlag = rhs._serverNameFlag;
	_rootFlag = rhs._rootFlag;
	_indexFlag = rhs._indexFlag;
	_methodFlag = rhs._methodFlag;
	_errorPageFlag = rhs._errorPageFlag;
	_locationFlag = rhs._locationFlag;
	_limitBodySizeFlag = rhs._limitBodySizeFlag;
	_cgiFlag = rhs._cgiFlag;


	_port = rhs._port;
	_serverName = rhs._serverName;
	_root = rhs._root;
	_index = rhs._index;
	_method = rhs._method;
	_errorPage = rhs._errorPage;
	_location = rhs._location;
	_limitBodySize = rhs._limitBodySize;
	_cgi = rhs._cgi;
	return *this;
}

Server::~Server()
{
}

void Server::printServer()
{
	std::cout << "port: ";
	for (size_t i = 0; i < _port.size(); i++)
		std::cout << _port[i] << " ";
	std::cout << std::endl;
	std::cout << "server_name: ";
	for (size_t i = 0; i < _serverName.size(); i++)
		std::cout << _serverName[i] << " ";
	std::cout << std::endl;
	std::cout << "root: " << _root << std::endl;
	std::cout << "index: ";
	for (size_t i = 0; i < _index.size(); i++)
		std::cout << _index[i] << " ";
	std::cout << std::endl;
	std::cout << "method: ";
	for (size_t i = 0; i < _method.size(); i++)
		std::cout << _method[i] << " ";
	std::cout << std::endl;
	std::cout << "error_page: ";
	for (std::map<int, std::string>::iterator it = _errorPage.begin(); it != _errorPage.end(); it++)
		std::cout << it->first << "=" << it->second << ", ";
	std::cout << std::endl;
	std::cout << "limit_body_size: " << _limitBodySize << std::endl;
	std::cout << "cgi: " << std::endl;
	for (size_t i = 0; i < _cgi.size(); i++)
	{
		for (size_t j = 0; j < _cgi[i].size(); j++)
			std::cout << _cgi[i][j] << " ";
		std::cout << std::endl;
	}
	for (size_t i = 0; i < _location.size(); i++)
	{
		std::cout << "location " << i << std::endl;
		_location[i].printLocation();
	}
}

void Server::parseServer(std::string block)
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
			try
			{
				_parsePort(issLine);
			}
			catch(const std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "server_name")
		{
			try
			{
				_parseServerName(issLine);
			}
			catch(const std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "root")
		{
			try
			{
				_parseRoot(issLine);
			}
			catch(const std::exception& e)
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
			catch(const std::exception& e)
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
			catch(const std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "error_page")
		{
			try
			{
				_parseErrorPage(issLine);
			}
			catch(const std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "limit_body_size")
		{
			try
			{
				_parseLimitBodySize(issLine);
			}
			catch(const std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "cgi")
		{
			try
			{
				_parseCgi(issLine);
			}
			catch(const std::exception& e)
			{
				throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
			}
		}
		else if (token == "location")
		{
			try
			{
				_parseLocation(iss, issLine);
			}
			catch (const std::exception& e)
			{
				throw std::runtime_error(e.what());
			}
		}
		else
		{
			throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
		}
	}
	for (size_t i = 0; i < _location.size(); i++)
	{
		_location[i].setLocation(_root, _index, _method);
	}
}

void Server::_parsePort(std::istringstream& issLine)
{
	if (_portFlag)
		throw std::runtime_error("listen directive is duplicated\n");
	else
	{
		_portFlag = true;
		_port.clear();
	}
	std::string token;
	while (issLine >> token && token != ";")
	{
		for (size_t i = 0; i < token.size(); i++)
		{
			if (!std::isdigit(token[i]))
				throw std::runtime_error("listen directive is invalid\n");
		}
		long port = strtol(token.c_str(), NULL, 10);
		if (port < 0 || port > 65535)
			throw std::runtime_error("listen directive is invalid\n");
		_port.push_back(port);
	}
	if (token != ";")
		throw std::runtime_error("listen directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("listen directive is invalid\n");
	if (_port.size() == 0)
		throw std::runtime_error("listen directive is invalid\n");
}

void Server::_parseServerName(std::istringstream& issLine)
{
	if (_serverNameFlag)
		throw std::runtime_error("server_name directive is duplicated\n");
	else
	{
		_serverNameFlag = true;
		_serverName.clear();
	}
	std::string token;
	while (issLine >> token && token != ";")
	{
		_serverName.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("server_name directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("server_name directive is invalid\n");
	if (_serverName.size() == 0)
		throw std::runtime_error("server_name directive is invalid\n");
}

void Server::_parseRoot(std::istringstream& issLine)
{
	if (_rootFlag)
		throw std::runtime_error("root directive is duplicated\n");
	else
		_rootFlag = true;
	std::string token;
	if (issLine >> token)
	{
		_root = token;
		std::string reservedChars = ":/?#[]@!$&'()*+,;=";
		std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
		for (size_t i = 0; i < _root.size(); i++)
		{
			if (reservedChars.find(_root[i]) == std::string::npos && unreservedChars.find(_root[i]) == std::string::npos)
				throw std::runtime_error("root directive is invalid\n");
		}
	}
	else
		throw std::runtime_error("root directive is invalid\n");
	issLine >> token;
	if (token != ";")
		throw std::runtime_error("root directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("root directive is invalid\n");
}

void Server::_parseIndex(std::istringstream& issLine)
{
	if (_indexFlag)
		throw std::runtime_error("index directive is duplicated\n");
	else
	{
		_indexFlag = true;
		_index.clear();
	}
	std::string token;
	while (issLine >> token && token != ";")
	{
		_index.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("index directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("index directive is invalid\n");
	if (_index.size() == 0)
		throw std::runtime_error("index directive is invalid\n");
}

void Server::_parseMethod(std::istringstream& issLine)
{
	if (_methodFlag)
		throw std::runtime_error("method directive is duplicated\n");
	else
	{
		_methodFlag = true;
		_method.clear();
	}
	std::string token;
	std::vector<std::string> validMethod;
	validMethod.push_back("GET");
	validMethod.push_back("POST");
	validMethod.push_back("DELETE");
	validMethod.push_back("PUT");
	validMethod.push_back("HEAD");
	while (issLine >> token && token != ";")
	{
		if (std::find(validMethod.begin(), validMethod.end(), token) == validMethod.end())
			throw std::runtime_error("method directive is invalid\n");
		_method.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("method directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("method directive is invalid\n");
	if (_method.size() == 0)
		throw std::runtime_error("method directive is invalid\n");
}

void Server::_parseErrorPage(std::istringstream& issLine)
{
	if (!_errorPageFlag)
	{
		_errorPageFlag = true;
		_errorPage.clear();
	}
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("error_page directive is invalid\n");
	for (size_t i = 0; i < token.size(); i++)
	{
		if (!std::isdigit(token[i]))
			throw std::runtime_error("error_page directive is invalid\n");
	}
	long errorCode = strtol(token.c_str(), NULL, 10);
	if (errorCode < 100 || errorCode > 599)
		throw std::runtime_error("error_page directive is invalid\n");
	if (!(issLine >> token))
		throw std::runtime_error("error_page directive is invalid\n");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.size(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("error_page directive is invalid\n");
	}
	_errorPage[errorCode] = token;
	if (issLine >> token && token != ";")
		throw std::runtime_error("error_page directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("error_page directive is invalid\n");
	if (_errorPage.size() == 0)
		throw std::runtime_error("error_page directive is invalid\n");
}

void Server::_parseLimitBodySize(std::istringstream& issLine)
{
	if (_limitBodySizeFlag)
		throw std::runtime_error("limit_body_size directive is duplicated\n");
	else
		_limitBodySizeFlag = true;
	std::string token;
	if (issLine >> token)
	{
		for (size_t i = 0; i < token.size(); i++)
		{
			if (!std::isdigit(token[i]))
				throw std::runtime_error("limit_body_size directive is invalid\n");
		}
		_limitBodySize = strtol(token.c_str(), NULL, 10);
	}
	else
		throw std::runtime_error("limit_body_size directive is invalid\n");
	issLine >> token;
	if (token != ";")
		throw std::runtime_error("limit_body_size directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("limit_body_size directive is invalid\n");
}

void Server::_parseLocation(std::istringstream& iss, std::istringstream& issLine)
{
	if (!_locationFlag)
	{
		_locationFlag = true;
		_location.clear();
	}
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("location directive is invalid\n");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.size(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("location directive is invalid\n");
	}
	std::string path = token;
	if (issLine >> token)
		throw std::runtime_error("location directive is invalid\n");
	std::string line;
	std::string block;
	if (std::getline(iss, line))
	{
		issLine.clear();
		issLine.str(line);
		issLine >> token;
		if (token != "{")
			throw std::runtime_error("location directive is invalid\n");
		if (issLine >> token)
			throw std::runtime_error("location directive is invalid\n");
	}
	while (std::getline(iss, line))
	{
		if (line.find("}") != std::string::npos)
			break;
		block += line + "\n";
	}
	Location location;
	try
	{
		location.parseLocation(block, path);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	_location.push_back(location);
}


std::vector<int>& Server::getPort()
{
	return _port;
}

std::vector<std::string>& Server::getServerName()
{
	return _serverName;
}

std::string& Server::getRoot()
{
	return _root;
}

std::vector<std::string>& Server::getIndex()
{
	return _index;
}

std::vector<std::string>& Server::getMethod()
{
	return _method;
}

std::map<int, std::string>& Server::getErrorPage()
{
	return _errorPage;
}

int& Server::getLimitBodySize()
{
	return _limitBodySize;
}

std::vector<Location>& Server::getLocation()
{
	return _location;
}

void Server::_parseCgi(std::istringstream& issLine)
{
	if (_cgiFlag)
		_cgiFlag = true;
	std::string token;
	std::vector<std::string> cgi;
	if (!(issLine >> token))
		throw std::runtime_error("cgi is not defined");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.length(); i++)
	{
		if (unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("cgi is invalid");
	}
	if (token[0] != '.')
		throw std::runtime_error("cgi is invalid");
	cgi.push_back(token);
	if (!(issLine >> token))
		throw std::runtime_error("cgi is invalid");
	for (size_t i = 0; i < token.length(); i++)
	{
		if (unreservedChars.find(token[i]) == std::string::npos && reservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("cgi is invalid");
	}
	cgi.push_back(token);
	if (!(issLine >> token))
		throw std::runtime_error("cgi is invalid");
	if (token != ";")
		throw std::runtime_error("cgi is invalid");
	if (issLine >> token)
		throw std::runtime_error("cgi is invalid");
	_cgi.push_back(cgi);
}

std::vector< std::vector<std::string> >& Server::getCgi()
{
	return _cgi;
}

bool& Server::getCgiFlag()
{
	return _cgiFlag;
}