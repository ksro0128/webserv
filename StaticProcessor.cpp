#include "StaticProcessor.hpp"

StaticProcessor::StaticProcessor()
{
}

StaticProcessor::~StaticProcessor()
{
}

void StaticProcessor::Set(Config& config, int kq)
{
	m_config = config;
	m_kq = kq;
}

void StaticProcessor::Process(Document& document)
{
	std::vector<Request>& requests = document.GetStatic();

	for (std::vector<Request>::iterator it = requests.begin(); it != requests.end(); it++)
	{
		processStatic(*it, document);
	}
	document.ClearStatic();
}

void StaticProcessor::processStatic(Request& request, Document& document)
{
	if (request.GetStatus() != 200)
	{
		// error
	}
	Server& server = m_config.GetServer(request.GetPort(), request.GetHost());
	Location& location = server.GetLocationBlock(request.GetPath());
	if (isAllowedMethod(request, location) == false || (request.GetMethod() != "GET" && request.GetMethod() != "HEAD"))
	{
		// 405 Method Not Allowed
	}
	document.ClearDynamic();
	// std::string path = getFilePath(request.GetPath(), location);
	// if (path == "")
	// {
	// 	// 404 Not Found
	// }
	// std::ifstream file(path.c_str());
	// if (file.is_open() == false)
	// {
	// 	// 404 Not Found
	// }
	// Response response;
	// response.SetStatusCode(200);
	// response.SetVersion(request.GetVersion());
	// response.SetOriginFd(request.GetFd());
	// std::ostringstream oss;
	// oss << file.rdbuf();
	// response.SetBody(oss.str());
	// struct kevent ev;
	// EV_SET(&ev, request.GetFd(), EVFILT_WRITE, EV_ADD, 0, 0, 0);
	// kevent(m_kq, &ev, 1, NULL, 0, NULL);
	// document.PutResponse(response);
	
}

bool StaticProcessor::isAllowedMethod(Request& request, Location& location)
{
	std::string method = request.GetMethod();
	std::vector<std::string> &methodSet = location.GetMethod();
	if (std::find(methodSet.begin(), methodSet.end(), method) == methodSet.end())
		return false;
	else
		return true;
}