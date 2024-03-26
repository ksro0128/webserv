#include "RequestProcessor.hpp"

RequestProcessor::RequestProcessor()
{
}

RequestProcessor::RequestProcessor(Config &config) : m_config(config)
{
}

RequestProcessor::~RequestProcessor()
{
}

void RequestProcessor::Set(Config &config, int kq)
{
	m_config = config;
	m_kq = kq;
}

void RequestProcessor::ProcessRequests(Document &document)
{
	std::vector<Request> &complete = document.getComplete();
	
	for (std::vector<Request>::iterator it = complete.begin(); it != complete.end(); it++)
	{
		processRequest(*it);
		std::cout << "Request processed" << std::endl;
	}
}

void RequestProcessor::processRequest(Request &request)
{
	request.getBody();
}