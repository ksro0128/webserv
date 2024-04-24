#include "Response.hpp"

Response::Response()
{
	m_version = "HTTP/1.1";
	m_statusCode = 200;
	m_statusMessage = "OK";
	m_body = "";
	m_flag = 0;
	m_start = 0;
	m_rest = -1;
}

Response::~Response()
{
}

Response::Response(const Response& rhs)
{
	m_version = rhs.m_version;
	m_statusCode = rhs.m_statusCode;
	m_statusMessage = rhs.m_statusMessage;
	m_headers = rhs.m_headers;
	m_body = rhs.m_body;
	m_origin_fd = rhs.m_origin_fd;
	m_flag = rhs.m_flag;
	m_responsemessage = rhs.m_responsemessage;
	m_start = rhs.m_start;
	m_rest = rhs.m_rest;
}

Response& Response::operator=(const Response& rhs)
{
	if (this == &rhs)
		return *this;
	m_version = rhs.m_version;
	m_statusCode = rhs.m_statusCode;
	m_statusMessage = rhs.m_statusMessage;
	m_headers = rhs.m_headers;
	m_body = rhs.m_body;
	m_origin_fd = rhs.m_origin_fd;
	m_flag = rhs.m_flag;
	m_responsemessage = rhs.m_responsemessage;
	m_start = rhs.m_start;
	m_rest = rhs.m_rest;
	return *this;
}

void Response::SetVersion(std::string version)
{
	m_version = version;
}

void Response::SetStatusCode(int statusCode)
{
	m_statusCode = statusCode;
}

void Response::SetStatusMessage(std::string statusMessage)
{
	m_statusMessage = statusMessage;
}

void Response::SetHeader(std::string key, std::string value)
{
	m_headers.insert(std::pair<std::string, std::string>(key, value));
}

void Response::SetBody(std::string body)
{
	m_body = body;
	std::ostringstream oss;
	oss << m_body.length();
	if (m_headers.find("Content-Length") != m_headers.end())
		m_headers.erase(m_headers.find("Content-Length"));
	m_headers.insert(std::pair<std::string, std::string>("Content-Length", oss.str()));
}

void Response::RemoveBody()
{
	m_body = "";
}

void Response::SetOriginFd(int origin_fd)
{
	m_origin_fd = origin_fd;
}

std::string Response::GetVersion()
{
	return m_version;
}

int Response::GetStatusCode()
{
	return m_statusCode;
}

std::string Response::GetStatusMessage()
{
	return m_statusMessage;
}

std::multimap<std::string, std::string>& Response::GetHeaders()
{
	return m_headers;
}

std::string Response::GetBody()
{
	return m_body;
}

std::string& Response::GetResponse()
{
	if (m_flag == 0)
	{
		std::string response = m_version + " " + intToString(m_statusCode) + " " + m_statusMessage + "\r\n";
		std::string date = makeDate();
		m_headers.insert(std::pair<std::string, std::string>("Date", date));
		m_headers.insert(std::pair<std::string, std::string>("Server", "Webserv"));

		for (std::multimap<std::string, std::string>::iterator it = m_headers.begin(); it != m_headers.end(); it++)
		{
			response += it->first + ": " + it->second + "\r\n";
		}
		response += "\r\n";
		response += m_body;
		// std::cout << "response is " << response << std::endl;
		m_responsemessage = response;
		m_start = 0;
		m_rest = response.length();
		// std::cout << "after set m_responsemessage\n";
		m_flag = 1;
		return m_responsemessage;
	}
	else
		return m_responsemessage;
}

std::string Response::makeDate()
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	std::string str(buffer);
	return str;
}

std::string Response::intToString(int n)
{
	std::string str;
	std::stringstream ss;
	ss << n;
	ss >> str;
	return str;
}

int Response::GetOriginFd()
{
	return m_origin_fd;
}

void Response::AddBody(std::string& body)
{
	m_body += body;
	std::ostringstream oss;
	oss << m_body.length();
	m_headers.insert(std::pair<std::string, std::string>("Content-Length", oss.str()));
}

std::string Response::GetHeader(const std::string& key)
{
	std::multimap<std::string, std::string>::iterator it = m_headers.find(key);
	if (it == m_headers.end())
		return "";
	return it->second;
}

void Response::RenewIndex(int len)
{
	m_start += len;
	m_rest -= len;
}

int Response::GetStartIndex()
{
	return m_start;
}

int Response::GetRestIndex()
{
	return m_rest;
}

void Response::PrintResponse()
{
	std::cout << "\033[1;33m";
	std::cout << "//////////////response start//////////////" << std::endl;
	size_t pos = m_responsemessage.find("\r\n\r\n");
	std::cout << m_responsemessage.substr(0, pos) << std::endl;
	std::cout << "\033[0m" << std::endl;
}

void Response::RenewHeader(std::string key, std::string value)
{
	std::multimap<std::string, std::string>::iterator it = m_headers.find(key);
	if (it != m_headers.end())
		m_headers.erase(it);
	m_headers.insert(std::pair<std::string, std::string>(key, value));
}