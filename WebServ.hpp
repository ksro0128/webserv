#pragma once

#include "main.hpp"
#include "Document.hpp"
#include "RequestMaker.hpp"


class WebServ
{
	public:
		WebServ();
		WebServ(std::string configPath);
		~WebServ();
		void runServer();
	private:
		WebServ(const WebServ&);
		WebServ& operator=(const WebServ&);
		Document _document;
		RequestMaker _requestmaker;
		Config _config;
		int openPort(int port);
};