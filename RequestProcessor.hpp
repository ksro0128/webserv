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
		std::map<int, std::string> m_statusMessageSet;

		void processRequest(Request &request, Document &document);
		void setResponseError(Request &request, Response &response, Server &server, int status);
		void processCgi(Request &request, Document &document, Server &server, std::vector<std::string>& cgi);
		void processStatic(Request &request, Document &document, Server &server);
		std::vector<std::string> isCgi(Request &request, Server &server);
		std::string getMimeType(std::string key, std::map<std::string, std::string> &mimeSet);
		std::string getExtension(std::string path);
		bool isAllowedMethod(Request &request, Location &location);
		std::string getFilePath(std::string path, Location &location);
};