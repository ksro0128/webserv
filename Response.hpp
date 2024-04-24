#pragma once

#include "include.hpp"

class Response
{
	public:
		Response();
		~Response();
		Response(const Response& rhs);
		Response& operator=(const Response& rhs);
		
		void SetVersion(std::string version);
		void SetStatusCode(int statusCode);
		void SetStatusMessage(std::string statusMessage);
		void SetHeader(std::string key, std::string value);
		void SetBody(std::string body);
		void RemoveBody(); // remove body (for HEAD method)
		void AddBody(std::string& body);
		void SetOriginFd(int origin_fd);
		void RenewIndex(int len);
		void RenewHeader(std::string key, std::string value);

		int GetStartIndex();
		int GetRestIndex();
		std::string GetVersion();
		int GetStatusCode();
		std::string GetStatusMessage();
		std::multimap<std::string, std::string>& GetHeaders();
		std::string GetHeader(const std::string& key); // get specific header value
		std::string GetBody();
		std::string& GetResponse();
		int GetOriginFd();


		void PrintResponse();

	private:
		std::string m_version;
		int m_origin_fd;
		int m_statusCode;
		int	m_flag;
		int m_start;
		int m_rest;

		std::string m_responsemessage;
		std::string m_statusMessage;
		std::multimap<std::string, std::string> m_headers;
		std::string m_body;
		std::string makeDate();
		std::string intToString(int n);
};