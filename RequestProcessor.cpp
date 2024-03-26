#include "RequestProcessor.hpp"

RequestProcessor::RequestProcessor()
{
}

RequestProcessor::RequestProcessor(Config &config) : _config(config)
{
}

RequestProcessor::~RequestProcessor()
{
}

void RequestProcessor::set(Config &config, int kq)
{
	_config = config;
	_kq = kq;
}

void RequestProcessor::processRequests(Document &document)
{
	std::vector<Request> &complete = document.GetComplete();
	
	for (std::vector<Request>::iterator it = complete.begin(); it != complete.end(); it++)
	{
		processRequest(*it);
		std::cout << "Request processed" << std::endl;
	}
}

void RequestProcessor::processRequest(Request &request)
{
	request.GetBody();
}