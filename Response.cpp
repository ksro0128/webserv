#include "Response.hpp"

Response::Response()
{
	_version = "HTTP/1.1";
	_statusCode = 200;
	_statusMessage = "OK";
	_body = "";
}

Response::~Response()
{
}

Response::Response(const Response& rhs)
{
	_version = rhs._version;
	_statusCode = rhs._statusCode;
	_statusMessage = rhs._statusMessage;
	_headers = rhs._headers;
	_body = rhs._body;
}

Response& Response::operator=(const Response& rhs)
{
	if (this == &rhs)
		return *this;
	_version = rhs._version;
	_statusCode = rhs._statusCode;
	_statusMessage = rhs._statusMessage;
	_headers = rhs._headers;
	_body = rhs._body;
	return *this;
}

void Response::setVersion(std::string version)
{
	_version = version;
}

void Response::setStatusCode(int statusCode)
{
	_statusCode = statusCode;
}

void Response::setStatusMessage(std::string statusMessage)
{
	_statusMessage = statusMessage;
}

void Response::setHeader(std::string key, std::string value)
{
	_headers.insert(std::pair<std::string, std::string>(key, value));
}

void Response::setBody(std::string body)
{
	_body = body;
}

std::string Response::getVersion()
{
	return _version;
}

int Response::getStatusCode()
{
	return _statusCode;
}

std::string Response::getStatusMessage()
{
	return _statusMessage;
}

std::multimap<std::string, std::string> Response::getHeaders()
{
	return _headers;
}

std::string Response::getBody()
{
	return _body;
}

std::string Response::getResponse()
{
	std::string response = _version + " " + intToString(_statusCode) + " " + _statusMessage + "\r\n";
	std::string date = makeDate();
	_headers.insert(std::pair<std::string, std::string>("Date", date));
	_headers.insert(std::pair<std::string, std::string>("Server", "Webserv"));
	_headers.insert(std::pair<std::string, std::string>("Content-Length", std::to_string(_body.length())));

	for (std::multimap<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n";
	response += _body;
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