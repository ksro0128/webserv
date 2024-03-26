#include "RequestProcessor.hpp"

RequestProcessor::RequestProcessor()
{
	m_statusMessageSet[100] = "Continue";
	m_statusMessageSet[101] = "Switching Protocols";
	m_statusMessageSet[200] = "OK";
	m_statusMessageSet[201] = "Created";
	m_statusMessageSet[202] = "Accepted";
	m_statusMessageSet[204] = "No Content";
	m_statusMessageSet[400] = "Bad Request";
	m_statusMessageSet[401] = "Unauthorized";
	m_statusMessageSet[403] = "Forbidden";
	m_statusMessageSet[404] = "Not Found";
	m_statusMessageSet[405] = "Method Not Allowed";
	m_statusMessageSet[406] = "Not Acceptable";
	m_statusMessageSet[408] = "Request Timeout";
	m_statusMessageSet[409] = "Conflict";
	m_statusMessageSet[411] = "Length Required";
	m_statusMessageSet[413] = "Payload Too Large";
	m_statusMessageSet[414] = "URI Too Long";
	m_statusMessageSet[415] = "Unsupported Media Type";
	m_statusMessageSet[500] = "Internal Server Error";
	m_statusMessageSet[501] = "Not Implemented";
	m_statusMessageSet[502] = "Bad Gateway";
	m_statusMessageSet[503] = "Service Unavailable";
	m_statusMessageSet[504] = "Gateway Timeout";
	m_statusMessageSet[505] = "HTTP Version Not Supported";
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
	std::vector<Request> &complete = document.GetComplete();
	
	for (std::vector<Request>::iterator it = complete.begin(); it != complete.end(); it++)
	{
		processRequest(*it, document);
	}
	document.RemoveComplete();
}

void RequestProcessor::processRequest(Request &request, Document &document)
{
	if (request.GetStatus() != 200)
	{
		return ;
	}
	std::string server_name = request.GetHost();
	int port = request.GetPort();
	Server &server = m_config.GetServer(port, server_name);

	if (server.GetCgiFlag() == false) // 정적
	{
		if (request.GetMethod() != "GET")
		{
			// 405 보내기
			return ;
		}
		Response response;
		response.SetVersion(request.GetVersion());
		response.SetStatusCode(200);
		response.SetStatusMessage("OK");
		response.SetHeader("Content-Type", "text/html");
		response.SetOriginFd(request.GetFd());
		std::string path = server.GetRoot() + request.GetPath();
		std::ifstream ifs(path);
		if (ifs.is_open() == false)
		{
			// 404 보내기
			std::cout << "404" << std::endl;
			return ;
		}
		std::ostringstream oss;
		oss << ifs.rdbuf();
		response.SetBody(oss.str());
		document.PutResponse(response);
		struct kevent ev;
		EV_SET(&ev, request.GetFd(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		kevent(m_kq, &ev, 1, NULL, 0, NULL);




		// std::string res = response.GetResponse();
		// std::cout << res << std::endl;
	}
	else // 동적
	{
		std::vector<std::vector<std::string> > &cgi = server.GetCgi();
		std::vector<std::vector<std::string> >::iterator it = cgi.begin();
		for (; it != cgi.end(); it++)
		{
			if (request.GetPath().find((*it)[0]) != std::string::npos)
			{
				break ;
			}
		}
	}
}