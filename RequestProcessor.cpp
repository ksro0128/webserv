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
		int status = request.GetStatus();
		Response response;
		setResponseError(request, response, m_config.GetServer(request.GetPort(), request.GetHost()), status);
		document.PutResponse(response);
		return ;
	}
	
	Server &server = m_config.GetServer(request.GetPort(), request.GetHost());
	if (server.GetCgiFlag() == false) // 정적 파일만 지원하는 서버
	{
		processStatic(request, document, server);
	}
	else // 동적 파일도 지원하는 서버
	{
		std::vector<std::string> cgi = isCgi(request, server);
		if (cgi.size() != 0)
		{
			
			processCgi(request, document, server, cgi);
		}
		else
		{
			processStatic(request, document, server);
		}
	}
}

void RequestProcessor::setResponseError(Request &request, Response &response, Server &server, int status)
{
	response.SetVersion("HTTP/1.1");
	response.SetStatusCode(status);
	response.SetStatusMessage(m_statusMessageSet[status]);
	response.SetOriginFd(request.GetFd());
	response.SetHeader("Content-Type", "text/html");
	std::map<int, std::string> &errorPage = server.GetErrorPage();
	std::string errorPath = errorPage[status];
	std::ifstream ifs(errorPath);
	std::ostringstream oss;
	oss << ifs.rdbuf();
	response.SetBody(oss.str());
	struct kevent ev;
	EV_SET(&ev, request.GetFd(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	kevent(m_kq, &ev, 1, NULL, 0, NULL);
}

void RequestProcessor::processStatic(Request &request, Document &document, Server &server)
{
	std::string method = request.GetMethod();
	Response response;
	Location &location = server.GetLocationBlock(request.GetPath());
	if (isAllowedMethod(request, location) == false || (method != "GET" && method != "HEAD"))
	{
		setResponseError(request, response, server, 405);
		std::vector<std::string> &methodSet = location.GetMethod();
		std::string allow;
		for (std::vector<std::string>::iterator it = methodSet.begin(); it != methodSet.end(); it++)
		{
			if (*it == "GET")
			{
				allow += "GET, ";
			}
			else if (*it == "HEAD")
			{
				allow += "HEAD, ";
			}
		}
		allow = allow.substr(0, allow.size() - 2);
		response.SetHeader("Allow", allow);
		document.PutResponse(response);
		return ;
	}
	std::string path = location.GetRoot() + request.GetPath();
	
	std::ifstream ifs(path);
	if (ifs.is_open() == false)
	{
		setResponseError(request, response, server, 404);
		document.PutResponse(response);
		return ;
	}
	response.SetVersion(request.GetVersion());
	response.SetStatusCode(200);
	response.SetStatusMessage("OK");
	response.SetHeader("Content-Type", getMimeType(getExtension(request.GetPath()), m_config.GetMimeSet()));
	response.SetOriginFd(request.GetFd());
	std::ostringstream oss;
	oss << ifs.rdbuf();
	if (method == "GET")
		response.SetBody(oss.str());
	struct kevent ev;
	EV_SET(&ev, request.GetFd(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	kevent(m_kq, &ev, 1, NULL, 0, NULL);
	document.PutResponse(response);
}

std::vector<std::string> RequestProcessor::isCgi(Request &request, Server &server)
{
	std::string extension = getExtension(request.GetPath());
	std::vector<std::string> cgi;
	std::vector< std::vector<std::string> > &cgiSet = server.GetCgi();
	for (std::vector< std::vector<std::string> >::iterator it = cgiSet.begin(); it != cgiSet.end(); it++)
	{
		if ((*it)[0] == extension)
		{
			cgi = *it;
			break;
		}
	}
	return cgi;
}


void RequestProcessor::processCgi(Request &request, Document &document, Server &server, std::vector<std::string>& cgi)
{
	request.GetFd();
	document.GetComplete();
	server.GetRoot();
	cgi[0];
	// std::cout << "cgi " << cgi[0] << " " << cgi[1] << std::endl;
}

std::string RequestProcessor::getExtension(std::string path)
{
	std::string key;
	size_t pos = path.find_last_of(".");
	if (pos != std::string::npos)
	{
		key = path.substr(pos + 1);
		pos = key.find_first_of("/");
		if (pos != std::string::npos)
			key.erase(pos);
	}
	else
	{
		key = "application/octet-stream";
	}
	return key;
}

std::string RequestProcessor::getMimeType(std::string key, std::map<std::string, std::string> &mimeSet)
{
	std::map<std::string, std::string>::iterator it = mimeSet.find(key);
	if (it == mimeSet.end())
	{
		return "application/octet-stream";
	}
	return it->second;
}

bool RequestProcessor::isAllowedMethod(Request& request, Location& location)
{
	std::string method = request.GetMethod();
	std::vector<std::string> &methodSet = location.GetMethod();
	if (std::find(methodSet.begin(), methodSet.end(), method) == methodSet.end())
		return false;
	else
		return true;
}

// std::string RequestProcessor::getFilePath(std::string path, Location &location)
// {
// 	// std::string path = location.GetRoot() + path;
// 	// stat으로 파일 존재 혹은 디렉토리인지 확인
// 	// 디렉토리면 index 받아서 stat확인
// 	// 다 돌고 없으면 autoindex on 인지 확인
// 	// autoindex on이면 디렉토리 내용 보여주기
// 	// 아니면 404
// }