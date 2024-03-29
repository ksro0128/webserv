#include "CgiProcessor.hpp"

CgiProcessor::CgiProcessor()
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

CgiProcessor::CgiProcessor(const CgiProcessor &other)
{
    *this = other;
}

CgiProcessor &CgiProcessor::operator=(const CgiProcessor &other)
{
    if (this != &other)
    {
    }
    return (*this);
}

CgiProcessor::~CgiProcessor()
{
}

void CgiProcessor::Set(Config &config, int kq)
{
    m_config = config;
    m_kq = kq;
}

void CgiProcessor::ExcuteCgi(Document &doc)
{
    for (std::vector<Request>::iterator i = doc.GetDynamic().begin(); i != doc.GetDynamic().end(); i++)
    {
        Request& request = *i;
        // std::string& method = request.GetMethod();
        Server& server = m_config.GetServer(request.GetPort(), request.GetHost());
        std::string filename = request.GetPath();
        std::vector<std::string> cgi = getCgiInfo(request, server);
        std::string extension = "." + cgi[0];
        unsigned long pos = filename.find_last_of("." + cgi[0]);
        filename = server.GetRoot() + filename.substr(0, pos + extension.length());
        std::ifstream ifs(filename);
        if (ifs.is_open() == false)
        {
        	Response response;
        	setResponseError(request, response, server, 404);
        	doc.PutResponse(response);
        	std::cout << "file open error" << std::endl;
        	return ;
        }
        if (access(cgi[1].c_str(), F_OK) == -1 || access(cgi[1].c_str(), X_OK) == -1)
        {
        	Response response;
        	setResponseError(request, response, server, 502);
        	doc.PutResponse(response);
        	std::cout << "Cgi path error" << std::endl;
        	return ;
        }
        int p[2];
        if (pipe(p) == -1)
        {
        	Response response;
        	setResponseError(request, response, server, 500);
        	doc.PutResponse(response);
        	return ;
        }
        int pid = fork();
        if (pid == -1)
        {
        	Response response;
        	setResponseError(request, response, server, 500);
        	doc.PutResponse(response);
        	return ;
        }
        if (pid == 0)
        {
        	close(p[0]);
        	// dup2(p[1], 1);
        	std::string path = server.GetRoot() + request.GetPath();
        	std::string query = request.GetQuery();
        	std::string body = request.GetBody();
            std::string method = request.GetMethod();
        	std::string contentLength = "CONTENT_LENGTH=" + std::to_string(body.length());
        	std::string queryString = "QUERY_STRING=" + query;
        	std::string requestMethod = "REQUEST_METHOD=" + method;
        	std::string scriptName = "SCRIPT_NAME=" + request.GetPath();
        	std::string scriptFilename = "SCRIPT_FILENAME=" + path;
        	std::string pathInfo = "PATH_INFO=" + request.GetPath();
        	std::string pathTranslated = "PATH_TRANSLATED=" + path;
        	std::string remoteAddr = "REMOTE_ADDR=" + request.GetHost();
        	std::string remotePort = "REMOTE_PORT=" + std::to_string(request.GetPort());
        	std::string serverName = "SERVER_NAME=" + server.GetServerName()[0];
        	std::string serverPort = "SERVER_PORT=" + std::to_string(server.GetPort()[0]);
        	std::string serverProtocol = "SERVER_PROTOCOL=" + request.GetVersion();
        	std::string serverSoftware = "SERVER_SOFTWARE=webserv";
        	char *envp[] = {
        		(char*)contentLength.c_str(),
        		// (char*)queryString.c_str(),
        		(char*)requestMethod.c_str(),
        		(char*)scriptName.c_str(),
        		(char*)scriptFilename.c_str(),
        		(char*)pathInfo.c_str(),
        		(char*)pathTranslated.c_str(),
        		(char*)remoteAddr.c_str(),
        		(char*)remotePort.c_str(),
        		(char*)serverName.c_str(),
        		(char*)serverPort.c_str(),
        		(char*)serverProtocol.c_str(),
        		(char*)serverSoftware.c_str(),
        		NULL
        	};
        	char *argv[] = {
        		(char*)filename.c_str(),
        		NULL
        	};
        	if (execve(cgi[1].c_str(), argv, envp) == -1)
            {
                std::cout << "execve error" << std::endl;
                exit(1);
            }
        }
        else
        {
        	close(p[1]);
            waitpid(pid, NULL, 0);
            close(p[0]);
        	// ExecInfo info(request.GetFd(), p[0], request);
        	// doc.PutExcute(pid, info);
        	// struct kevent ev;
        	// EV_SET(&ev, p[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
        }
    }
    doc.ClearDynamic();
}

std::vector<std::string> CgiProcessor::getCgiInfo(Request &request, Server &server)
{
    std::string extension = request.GetPath();
    unsigned long pos = extension.find_last_of(".");
    unsigned long end = extension.find_last_of("/");
    if (end != std::string::npos)
        extension = extension.substr(pos + 1, end - pos - 1);
    else
        extension = extension.substr(pos + 1, extension.length() - pos - 1);
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


void CgiProcessor::setResponseError(Request &request, Response &response, Server &server, int status)
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


