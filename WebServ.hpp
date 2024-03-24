#pragma once

#include "main.hpp"

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
		Config _config;
		int openPort(int port);
};