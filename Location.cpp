#include "Location.hpp"

Location::Location()
{
	m_path = "/";
	m_root = "";
	m_index.clear();
	m_method.clear();
	m_autoIndex = false;
	m_redirectionCode = 0;
	m_redirectionUri = "";
	m_rootFlag = false;
	m_indexFlag = false;
	m_methodFlag = false;
	m_autoIndexFlag = false;
	m_redirectionFlag = false;
	m_limitbodysizeFlag = false;
}

Location::Location(const Location& rhs)
{
	*this = rhs;
}

Location& Location::operator=(const Location& rhs)
{
	if (this == &rhs)
		return *this;
	m_path = rhs.m_path;
	m_root = rhs.m_root;
	m_index = rhs.m_index;
	m_method = rhs.m_method;
	m_autoIndex = rhs.m_autoIndex;
	m_redirectionCode = rhs.m_redirectionCode;
	m_redirectionUri = rhs.m_redirectionUri;
	m_limitbodysize = rhs.m_limitbodysize;
	m_rootFlag = rhs.m_rootFlag;
	m_indexFlag = rhs.m_indexFlag;
	m_methodFlag = rhs.m_methodFlag;
	m_autoIndexFlag = rhs.m_autoIndexFlag;
	m_redirectionFlag = rhs.m_redirectionFlag;
	m_limitbodysizeFlag = rhs.m_limitbodysizeFlag;

	return *this;
}

Location::~Location()
{
}

void Location::PrintLocation()
{
	std::cout << "path: " << m_path << std::endl;
	std::cout << "root: " << m_root << std::endl;
	std::cout << "index: ";
	for (size_t i = 0; i < m_index.size(); i++)
		std::cout << m_index[i] << " ";
	std::cout << std::endl;
	std::cout << "method: ";
	for (size_t i = 0; i < m_method.size(); i++)
		std::cout << m_method[i] << " ";
	std::cout << std::endl;
	std::cout << "autoindex: ";
	if (m_autoIndex)
		std::cout << "on" << std::endl;
	else
		std::cout << "off" << std::endl;
	std::cout << "redirectionCode: " << m_redirectionCode << std::endl;
	std::cout << "redirectionUri: " << m_redirectionUri << std::endl;

}

void Location::ParseLocation(std::string block, std::string path)
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
				parseRoot(issLine);
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
				parseIndex(issLine);
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
				parseMethod(issLine);
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
				parseRedirection(issLine);
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
				parseAutoIndex(issLine);
			}
			catch (std::exception& e)
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
		else
			throw std::runtime_error("Error: config file is invalid\n>>>" + line + "<<<");
	}
	m_path = path;
}

void Location::parseRoot(std::istringstream& issLine)
{
	if (m_rootFlag)
		throw std::runtime_error("root is already defined");
	m_rootFlag = true;
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
	m_root = token;
	if (!(issLine >> token))
		throw std::runtime_error("root is invalid");
	if (token != ";")
		throw std::runtime_error("root is invalid");
	if (issLine >> token)
		throw std::runtime_error("root is invalid");
}

void Location::parseIndex(std::istringstream& issLine)
{
	if (m_indexFlag)
		throw std::runtime_error("index is already defined");
	m_indexFlag = true;
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
		m_index.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("index is invalid");
	if (issLine >> token)
		throw std::runtime_error("index is invalid");
}

void Location::parseMethod(std::istringstream& issLine)
{
	if (m_methodFlag)
		throw std::runtime_error("method is already defined");
	m_methodFlag = true;
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
		m_method.push_back(token);
	}
	if (token != ";")
		throw std::runtime_error("method is invalid");
	if (issLine >> token)
		throw std::runtime_error("method is invalid");
}

void Location::parseAutoIndex(std::istringstream& issLine)
{
	if (m_autoIndexFlag)
		throw std::runtime_error("autoindex is already defined");
	m_autoIndexFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("autoindex is not defined");
	if (token == "on")
		m_autoIndex = true;
	else if (token == "off")
		m_autoIndex = false;
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
	if (m_redirectionFlag)
		throw std::runtime_error("return is already defined");
	m_redirectionFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("return is not defined");
	if (token != "301" && token != "302" && token != "303" && token != "307" && token != "308")
		throw std::runtime_error("return is invalid");
	m_redirectionCode = strtol(token.c_str(), NULL, 10);
	if (!(issLine >> token))
		throw std::runtime_error("return is invalid");
	std::string reservedChars = ":/?#[]@!$&'()*+,;=";
	std::string unreservedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
	for (size_t i = 0; i < token.length(); i++)
	{
		if (reservedChars.find(token[i]) == std::string::npos && unreservedChars.find(token[i]) == std::string::npos)
			throw std::runtime_error("return is invalid");
	}
	m_redirectionUri = token;
	if (!(issLine >> token))
		throw std::runtime_error("return is invalid");
	if (token != ";")
		throw std::runtime_error("return is invalid");
	if (issLine >> token)
		throw std::runtime_error("return is invalid");
}

void Location::SetLocation(std::string root, std::vector<std::string> index, std::vector<std::string> method)
{
	if (!m_rootFlag)
		m_root = root;
	if (!m_indexFlag)
		m_index = index;
	if (!m_methodFlag)
		m_method = method;
}

std::string& Location::GetPath()
{
	return m_path;
}

std::string& Location::GetRoot()
{
	return m_root;
}

std::vector<std::string>& Location::GetIndex()
{
	return m_index;
}

std::vector<std::string>& Location::GetMethod()
{
	return m_method;
}


bool& Location::GetAutoIndex()
{
	return m_autoIndex;
}

int& Location::GetRedirectionCode()
{
	return m_redirectionCode;
}

std::string& Location::GetRedirectionUri()
{
	return m_redirectionUri;
}

bool& Location::GetRedicetionFlag()
{
	return m_redirectionFlag;
}

void Location::parseLimitBodySize(std::istringstream& issLine)
{
	if (m_limitbodysizeFlag)
		throw std::runtime_error("limit_body_size is already defined");
	m_limitbodysizeFlag = true;
	std::string token;
	if (!(issLine >> token))
		throw std::runtime_error("limit_body_size is not defined");
	m_limitbodysize = strtol(token.c_str(), NULL, 10);
	if (!(issLine >> token))
		throw std::runtime_error("limit_body_size is invalid");
	if (token != ";")
		throw std::runtime_error("limit_body_size is invalid");
	if (issLine >> token)
		throw std::runtime_error("limit_body_size is invalid");
}

size_t Location::GetLimitBodySize()
{
	return m_limitbodysize;
}