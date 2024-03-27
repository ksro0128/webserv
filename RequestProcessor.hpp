#pragma once

#include "include.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Document.hpp"

class RequestProcessor
{
	public:
		RequestProcessor();
		RequestProcessor(Config &config);
		~RequestProcessor();
		void Set(Config &config, int kq);
		void ProcessRequests(Document &document);
	private:
		RequestProcessor(const RequestProcessor &src);
		RequestProcessor &operator=(const RequestProcessor &src);
		Config m_config;
		int m_kq;
		void processRequest(Request &request, Document &document);
		std::map<int, std::string> m_statusMessageSet;
		void setResponseError(Response &response, Server &server, int status);
		void setWriteable();
		void processCgi(Request &request, Document &document, Server &server, std::vector<std::string>& cgi);
};