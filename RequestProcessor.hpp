#pragma once

#include "include.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Document.hpp"

class RequestProcessor
{
	public:
		RequestProcessor(Config &config);
		~RequestProcessor();
		void processRequests(Document &document, int kq);
	private:
		RequestProcessor();
		RequestProcessor(const RequestProcessor &src);
		RequestProcessor &operator=(const RequestProcessor &src);
		Config _config;
		int _kq;
		void processRequest(Request &request);

};