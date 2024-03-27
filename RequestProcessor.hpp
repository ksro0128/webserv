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
		// std::string getPath(std::string path, Server &server); // location 받아서 돌리면서 맞는 location 찾기 class server에서 할까.. location 반환하는 함수 만들어야겠다
};