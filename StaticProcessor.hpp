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
		// std::string getFilePath(std::string path, Location& location);
};