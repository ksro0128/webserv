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

// 오늘 정적 파일 처리 끝내고 에러 405, 404 처리 확인 하기
// 동적 파일 구조 짜놓기

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
	// method가 get, head가 아니면 405 보내기 -> header에 Allow: 허용 method들 넣어주기
	// 서버에서 지원하는 method에 request의 method가 있는지 확인 - 없으면 405 보내기
	// path에 해당하는 파일이 없으면 404 보내기
	// 파일이 있으면 200 보내기
	// 파일 읽어서 body에 넣어주기
	// response에 version, statuscode, statusmessage, header, body 넣어주기
	// kevent로 writeable 설정
	// document에 response 넣어주기
	std::string method = request.GetMethod();
	std::vector<std::string> &methodSet = server.GetMethod();
	Response response;

	if (std::find(methodSet.begin(), methodSet.end(), method) == methodSet.end() || (method != "GET" && method != "HEAD"))
	{
		setResponseError(request, response, server, 405);
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
	std::string path = server.GetRoot() + request.GetPath();
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
	response.SetHeader("Content-Type", getMimeType(getExtension(path), m_config.GetMimeSet()));
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
	std::cout << "cgi " << cgi[0] << " " << cgi[1] << std::endl;
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