#pragma once

#include "include.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Document.hpp"


class StaticProcessor
{
	public:
		StaticProcessor();
		~StaticProcessor();
		void Process(Document& document);
		void Set(Config& config, int kq);
	private:
		StaticProcessor(const StaticProcessor& rhs);
		StaticProcessor& operator=(const StaticProcessor& rhs);
		Config m_config;
		int m_kq;

		void processStatic(Request& request, Document& document);
		bool isAllowedMethod(Request& request, Location& location);
		void setResponse(Request &request, Response &response, int status);
		void putBody(Response& response, std::string path);
		void putAllowedMethod(Response& response, Location& location);
		void setEventWriteable(int fd);
		std::string getFilePath(Document& document, Request &request, Response &response, Server &server, Location& location);
		bool isCgi(std::string path, Server& server);
		std::string autoIndex(std::string path);
};