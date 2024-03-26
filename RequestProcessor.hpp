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
		void set(Config &config, int kq);
		void processRequests(Document &document);
	private:
		RequestProcessor(const RequestProcessor &src);
		RequestProcessor &operator=(const RequestProcessor &src);
		Config _config;
		int _kq;
		void processRequest(Request &request);

};