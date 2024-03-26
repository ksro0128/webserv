#pragma once

#include "include.hpp"

class Response
{
	public:
		Response();
		~Response();
		Response(const Response& rhs);
		Response& operator=(const Response& rhs);
		
		void setVersion(std::string version);
		void setStatusCode(int statusCode);
		void setStatusMessage(std::string statusMessage);
		void setHeader(std::string key, std::string value);
		void setBody(std::string body);
		
		std::string getVersion();
		int getStatusCode();
		std::string getStatusMessage();
		std::multimap<std::string, std::string> getHeaders();
		std::string getBody();
		std::string getResponse();	

	private:
		std::string _version;
		int _statusCode;
		std::string _statusMessage;
		std::multimap<std::string, std::string> _headers;
		std::string _body;
		std::string makeDate();
		std::string intToString(int n);
};