#include "Server.hpp"

Server::Server()
{
	m_portFlag = false;
	m_serverNameFlag = false;
	m_rootFlag = false;
	m_indexFlag = false;
	m_methodFlag = false;
	m_errorPageFlag = false;
	m_locationFlag = false;
	m_limitBodySizeFlag = false;
	m_cgiFlag = false;
	m_port.push_back(80);
	m_serverName.push_back("localhost");
	m_root = ".";
	m_index.push_back("index.html");
	m_method.push_back("GET");
	m_method.push_back("POST");
	m_method.push_back("DELETE");
	m_errorPage[400] = "./errorpage/400.html";
	m_location.push_back(Location());
	m_limitBodySize = 1000000;
	m_cgi.clear();
}

Server::Server(const Server& rhs)
{
	*this = rhs;
}

Server& Server::operator=(const Server& rhs)
{
	if (this == &rhs)
		return *this;
	m_portFlag = rhs.m_portFlag;
	m_serverNameFlag = rhs.m_serverNameFlag;
	m_rootFlag = rhs.m_rootFlag;
	m_indexFlag = rhs.m_indexFlag;
	m_methodFlag = rhs.m_methodFlag;
	m_errorPageFlag = rhs.m_errorPageFlag;
	m_locationFlag = rhs.m_locationFlag;
	m_limitBodySizeFlag = rhs.m_limitBodySizeFlag;
	m_cgiFlag = rhs.m_cgiFlag;


	m_port = rhs.m_port;
	m_serverName = rhs.m_serverName;
	m_root = rhs.m_root;
	m_index = rhs.m_index;
	m_method = rhs.m_method;
	m_errorPage = rhs.m_errorPage;
	m_location = rhs.m_location;
	m_limitBodySize = rhs.m_limitBodySize;
	m_cgi = rhs.m_cgi;
	return *this;
}

Server::~Server()
{
}

void Server::PrintServer()
{
	std::cout << "port: ";
	for (size_t i = 0; i < m_port.size(); i++)
		std::cout << m_port[i] << " ";
	std::cout << std::endl;
	std::cout << "server_name: ";
	for (size_t i = 0; i < m_serverName.size(); i++)
		std::cout << m_serverName[i] << " ";
	std::cout << std::endl;
	std::cout << "root: " << m_root << std::endl;
	std::cout << "index: ";
	for (size_t i = 0; i < m_index.size(); i++)
		std::cout << m_index[i] << " ";
	std::cout << std::endl;
	std::cout << "method: ";
	for (size_t i = 0; i < m_method.size(); i++)
		std::cout << m_method[i] << " ";
	std::cout << std::endl;
	std::cout << "error_page: ";
	for (std::map<int, std::string>::iterator it = m_errorPage.begin(); it != m_errorPage.end(); it++)
		std::cout << it->first << "=" << it->second << ", ";
	std::cout << std::endl;
	std::cout << "limit_body_size: " << m_limitBodySize << std::endl;
	std::cout << "cgi: " << std::endl;
	for (size_t i = 0; i < m_cgi.size(); i++)
	{
		for (size_t j = 0; j < m_cgi[i].size(); j++)
			std::cout << m_cgi[i][j] << " ";
		std::cout << std::endl;
	}
	for (size_t i = 0; i < m_location.size(); i++)
	{
		std::cout << "location " << i << std::endl;
		m_location[i].PrintLocation();
	}
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
			try
			{
				parsePort(issLine);
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
				parseServerName(issLine);
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
				parseRoot(issLine);
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
				parseIndex(issLine);
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
				parseMethod(issLine);
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
				parseErrorPage(issLine);
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
				parseLimitBodySize(issLine);
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
				parseCgi(issLine);
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
				parseLocation(iss, issLine);
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
	for (size_t i = 0; i < m_location.size(); i++)
	{
		m_location[i].SetLocation(m_root, m_index, m_method);
	}
}

void Server::parsePort(std::istringstream& issLine)
{
	if (m_portFlag)
		throw std::runtime_error("listen directive is duplicated\n");
	else
	{
		m_portFlag = true;
		m_port.clear();
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
		m_port.push_back(port);
	}
	if (token != ";")
		throw std::runtime_error("listen directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("listen directive is invalid\n");
	if (m_port.size() == 0)
		throw std::runtime_error("listen directive is invalid\n");
}

void Server::parseServerName(std::istringstream& issLine)
{
	if (m_serverNameFlag)
		throw std::runtime_error("server_name directive is duplicated\n");
	else
	{
		m_serverNameFlag = true;
		m_serverName.clear();
	}
	std::string token;
	while (issLine >> token && token != ";")
	{
		m_serverName.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("server_name directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("server_name directive is invalid\n");
	if (m_serverName.size() == 0)
		throw std::runtime_error("server_name directive is invalid\n");
}

void Server::parseRoot(std::istringstream& issLine)
{
	if (m_rootFlag)
		throw std::runtime_error("root directive is duplicated\n");
	else
		m_rootFlag = true;
	std::string token;
	if (issLine >> token)
	{
		m_root = token;
		std::string reservedChars = ":/?#[]@!$&'()*+,;=";
		std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
		for (size_t i = 0; i < m_root.size(); i++)
		{
			if (reservedChars.find(m_root[i]) == std::string::npos && unreservedChars.find(m_root[i]) == std::string::npos)
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

void Server::parseIndex(std::istringstream& issLine)
{
	if (m_indexFlag)
		throw std::runtime_error("index directive is duplicated\n");
	else
	{
		m_indexFlag = true;
		m_index.clear();
	}
	std::string token;
	while (issLine >> token && token != ";")
	{
		m_index.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("index directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("index directive is invalid\n");
	if (m_index.size() == 0)
		throw std::runtime_error("index directive is invalid\n");
}

void Server::parseMethod(std::istringstream& issLine)
{
	if (m_methodFlag)
		throw std::runtime_error("method directive is duplicated\n");
	else
	{
		m_methodFlag = true;
		m_method.clear();
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
		m_method.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("method directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("method directive is invalid\n");
	if (m_method.size() == 0)
		throw std::runtime_error("method directive is invalid\n");
}

void Server::parseErrorPage(std::istringstream& issLine)
{
	if (!m_errorPageFlag)
	{
		m_errorPageFlag = true;
		m_errorPage.clear();
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
	m_errorPage[errorCode] = token;
	if (issLine >> token && token != ";")
		throw std::runtime_error("error_page directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("error_page directive is invalid\n");
	if (m_errorPage.size() == 0)
		throw std::runtime_error("error_page directive is invalid\n");
}

void Server::parseLimitBodySize(std::istringstream& issLine)
{
	if (m_limitBodySizeFlag)
		throw std::runtime_error("limit_body_size directive is duplicated\n");
	else
		m_limitBodySizeFlag = true;
	std::string token;
	if (issLine >> token)
	{
		for (size_t i = 0; i < token.size(); i++)
		{
			if (!std::isdigit(token[i]))
				throw std::runtime_error("limit_body_size directive is invalid\n");
		}
		m_limitBodySize = strtol(token.c_str(), NULL, 10);
	}
	else
		throw std::runtime_error("limit_body_size directive is invalid\n");
	issLine >> token;
	if (token != ";")
		throw std::runtime_error("limit_body_size directive is invalid\n");
	if (issLine >> token)
		throw std::runtime_error("limit_body_size directive is invalid\n");
}

void Server::parseLocation(std::istringstream& iss, std::istringstream& issLine)
{
	if (!m_locationFlag)
	{
		m_locationFlag = true;
		m_location.clear();
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
		location.ParseLocation(block, path);
	}
	catch(const std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	m_location.push_back(location);
}


std::vector<int>& Server::GetPort()
{
	return m_port;
}

std::vector<std::string>& Server::GetServerName()
{
	return m_serverName;
}

std::string& Server::GetRoot()
{
	return m_root;
}

std::vector<std::string>& Server::GetIndex()
{
	return m_index;
}

std::vector<std::string>& Server::GetMethod()
{
	return m_method;
}

std::map<int, std::string>& Server::GetErrorPage()
{
	return m_errorPage;
}

int& Server::GetLimitBodySize()
{
	return m_limitBodySize;
}

std::vector<Location>& Server::GetLocation()
{
	return m_location;
}

void Server::parseCgi(std::istringstream& issLine)
{
	if (!m_cgiFlag)
		m_cgiFlag = true;
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
	m_cgi.push_back(cgi);
}

std::vector< std::vector<std::string> >& Server::GetCgi()
{
	return m_cgi;
}

bool& Server::GetCgiFlag()
{
	return m_cgiFlag;
}