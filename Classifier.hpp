#pragma once

#include "include.hpp"
#include "Document.hpp"
#include "Server.hpp"
#include "Config.hpp"


class Classifier
{
	public:
		Classifier();
		void Set(Config& config);
		~Classifier();
		void Classify(Document& document);
	private:
		Classifier(const Classifier& rhs);
		Classifier& operator=(const Classifier& rhs);
		Config m_config;
		void classifyRequest(Document& document, Request& request);
		std::string getExtension(std::string path);
		bool isCgi(Request& request, Server& server);
};