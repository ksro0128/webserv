#include "Response.hpp"

Response::Response()
{
	m_version = "HTTP/1.1";
	m_statusCode = 200;
	m_statusMessage = "OK";
	m_body = "";
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

std::multimap<std::string, std::string> Response::GetHeaders()
{
	return m_headers;
}

std::string Response::GetBody()
{
	return m_body;
}

std::string Response::GetResponse()
{
	std::string response = m_version + " " + intToString(m_statusCode) + " " + m_statusMessage + "\r\n";
	std::string date = makeDate();
	m_headers.insert(std::pair<std::string, std::string>("Date", date));
	m_headers.insert(std::pair<std::string, std::string>("Server", "Webserv"));
	m_headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(m_body.length())));

	for (std::multimap<std::string, std::string>::iterator it = m_headers.begin(); it != m_headers.end(); it++)
	{
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n";
	response += m_body;
	return response;
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