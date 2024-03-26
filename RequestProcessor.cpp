#include "RequestProcessor.hpp"

RequestProcessor::RequestProcessor(Config &config) : _config(config)
{
}

RequestProcessor::~RequestProcessor()
{
}

void RequestProcessor::processRequests(Document &document, int kq)
{
	_kq = kq;
	std::vector<Request> &complete = document.getComplete();
	
	for (std::vector<Request>::iterator it = complete.begin(); it != complete.end(); it++)
	{
		processRequest(*it);
	}
}

void RequestProcessor::processRequest(Request &request)
{
	
}