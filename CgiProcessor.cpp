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

void CgiProcessor::Set(Config &config, int kq, std::vector<std::string> envp)
{
    m_config = config;
    m_kq = kq;
	m_envp = envp;
}

void CgiProcessor::ExcuteCgi(Document &doc)
{
    for (std::vector<Request>::iterator i = doc.GetDynamic().begin(); i != doc.GetDynamic().end(); i++)
    {
        Request& request = *i;
        // std::string& method = request.GetMethod();
        Server& server = m_config.GetServer(request.GetPort(), request.GetHost());
		Location& location = server.GetLocationBlock(request.GetPath());
		if (isAllowedMethod(request, location) == false)
		{
			Response response;
			setResponseError(request, response, server, 405);
			doc.PutResponse(response);
			setWriteEvent(request.GetFd());
			continue;
		}
        std::string filename = request.GetPath();
        std::vector<std::string> cgi = getCgiInfo(request, server);
        std::string extension = "." + cgi[0];
        unsigned long pos = filename.find_last_of("." + cgi[0]);
        filename = location.GetRoot() + filename.substr(0, pos + extension.length());
		// method check
		// std::cout << "cgi path : " << cgi[1] << std::endl;
		// std::cout << "filename : " << filename << std::endl;
        std::ifstream ifs(filename);
        if (ifs.is_open() == false)
        {
			if (request.GetMethod() == "POST")
			{
				std::cout << "its post!\n";
			}
			else
			{
				Response response;
				setResponseError(request, response, server, 404);
				doc.PutResponse(response);
				setWriteEvent(request.GetFd());
				std::cout << "file open error" << std::endl;
				continue ;
			}
        }
        if (access(cgi[1].c_str(), F_OK) == -1 || access(cgi[1].c_str(), X_OK) == -1)
        {
        	Response response;
        	setResponseError(request, response, server, 502);
        	doc.PutResponse(response);
			setWriteEvent(request.GetFd());
        	std::cout << "Cgi path error" << std::endl;
        	continue;
        }
        int p1[2];
		int p2[2];
        if (pipe(p1) == -1 || pipe(p2) == -1)
        {
        	Response response;
        	setResponseError(request, response, server, 500);
        	doc.PutResponse(response);
			setWriteEvent(request.GetFd());
        	continue;
        }
        int pid = fork();
        if (pid == -1)
        {
        	Response response;
        	setResponseError(request, response, server, 500);
        	doc.PutResponse(response);
			setWriteEvent(request.GetFd());
        	continue;
        }
        if (pid == 0)
			inChild(request, server, cgi, filename, p1, p2);
        else
        {
			close(p1[0]);
			close(p2[1]);
        	ExecInfo *p = new ExecInfo(request.GetFd(), p2[0], p1[1], pid, request);
			std::hash<ExecInfo*> hash;
			size_t key = hash(p);
			setReadEvent(p2[0]);
			fcntl(p2[0], F_SETFL, O_NONBLOCK);
			fcntl(p1[1], F_SETFL, O_NONBLOCK);
			if (request.GetBody().length() > 0)
			{
				std::cout << "there is body to write cgi and write pipe is " << p1[1] << std::endl;
				setWriteEvent(p1[1]);
				doc.PutPipeInfo(p1[1], key);
			}
			else
				close(p1[1]);
			setPidEvent(pid);
			doc.PutExcuteInfo(key, p);
			doc.PutPipeInfo(p2[0], key);
			doc.PutPidInfo2(pid, key);
			// std::cout << "excute cgi! client fd is" << request.GetFd() << " and write pipe is" << p1[1] << " read pipe is " << p2[0] << std::endl;
        }
    }
    doc.ClearDynamic();
}

void CgiProcessor::Write(Document &doc, int fd)
{
	size_t hash = doc.GetHashByPipe(fd);
	ExecInfo* info = doc.GetExcuteInfoByHash(hash);
	Request& request = info->GetRequest();
	// std::string& body = request.GetBody();
	// std::cout << "lets write cgi body length is " << body.length() << std::endl;
	int& start = info->GetPointer();
	int& len = info->GetBodyLen();
	const char* body = request.GetBody().c_str();
	if (len != 0)
	{
		int writeLen;
		if (len > 65536)
			writeLen = write(fd, &body[start], 65536);
		else
			writeLen = write(fd, &body[start], len);
		if (writeLen == 0)
		{
			// std::cout << "cause read pipe is closed, write is end\n";
			close(fd);
			doc.RemovePipeInfo(fd);
			return;
		}
		else if (writeLen == -1)
			return ;
		else
		{
			// std::cout << "success write " << writeLen << " byte\n";
			start += writeLen;
			len -= writeLen;
		}
	}
	else
	{
		// std::cout << "there is no body to write cgi so write is end\n";
		close(fd);
		doc.RemovePipeInfo(fd);
	}
}

void CgiProcessor::Read(Document &doc, int fd)
{
	// static int flag = 0;
	char buf[65537];
	int len;
	len = read(fd, buf, 65536);
	if (len == -1)
	{
		// std::cout << "read error pipe is " << fd << std::endl;
		close(fd);
		doc.RemovePipeInfo(fd);
		return;
	}
	else if (len == 0)
	{
		// std::cout << "cause write pipe is closed, read is end\n";
		close(fd);
		doc.RemovePipeInfo(fd);
		return;
	}
	buf[len] = '\0';
	// std::cout << buf[0];
	size_t hash = doc.GetHashByPipe(fd);
	// std::cout << "read len is " << len << std::endl;
	// std::cout << "in cgi read, fd is " << fd << " and read len is " << len << " and hash is " << hash << std::endl;
	doc.GetExcuteInfoByHash(hash)->GetBuffer() += buf;
	// if (flag == 0 && doc.GetExcuteInfoByHash(hash)->GetBuffer().length() > 100000)
	// {
	// 	std::string& body = doc.GetExcuteInfoByHash(hash)->GetBuffer();
	// 	body = body.substr(0, 58);
	// 	std::cout << "\n\n\n\n\ncut!!!\n\n\n\n\n";
	// 	std::cout << "cutted body is " << doc.GetExcuteInfoByHash(hash)->GetBuffer() << std::endl;
	// 	close(fd);
	// 	doc.RemovePipeInfo(fd);
	// 	flag = 1;
	// }
}

void CgiProcessor::Wait(Document &doc, int pid)
{
	int status;
	waitpid(pid, &status, 0);
	// std::cout << "wait pid is " << pid << std::endl;
	size_t hash = doc.GetHashByPid(pid);
	ExecInfo* info = doc.GetExcuteInfoByHash(hash);
	Request& request = info->GetRequest();
	Server& server = m_config.GetServer(request.GetPort(), request.GetHost());
	Response response;
	int pipefd = info->GetReadPipe();
	sockaddr_in addr;
	socklen_t len = sizeof(addr);
	// std::cout << "buffer length is " << info->GetBuffer().length() << std::endl;	
	if (WIFEXITED(status))
	{
		response.SetVersion("HTTP/1.1");
		response.SetStatusCode(200);
		response.SetStatusMessage("OK");
		response.SetOriginFd(request.GetFd());
		response.SetHeader("Content-Type", "text/html");
		parseCgiBody(response, info->GetBuffer());
		response.SetBody(info->GetBuffer());
		if (request.GetMethod() == "HEAD")
			response.RemoveBody();
	}
	else
	{
		setResponseError(request, response, server, 500);
	}
	close(pipefd);
	if (getsockname(request.GetFd(), (struct sockaddr*)&addr, &len) == 0)
	{
		// std::cout << "lets put response and register to write event\n";
		doc.PutResponse(response);
		setWriteEvent(request.GetFd());
	}
	else
		std::cout << "client socket is closed\n";
	doc.RemoveExcuteInfo(hash);
	doc.RemovePipeInfo(pipefd);
	doc.RemovePidInfo2(pid);
	// std::cout << "size of excute info is " << doc.GetExcuteCount() << std::endl;
	// std::cout << "size of pipe info is " << doc.GetPipeInfo().size() << std::endl;
	// std::cout << "first member of pipe info is " << doc.GetPipeInfo().begin()->first << std::endl;
	// std::cout << "size of pid info is " << doc.GetPidInfo2().size() << std::endl;
	// std::cout << "end wait\n";
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
}

void CgiProcessor::setWriteEvent(int fd)
{
	struct kevent ev;
	EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	kevent(m_kq, &ev, 1, NULL, 0, NULL);
}

void CgiProcessor::setReadEvent(int fd)
{
	struct kevent ev;
	EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(m_kq, &ev, 1, NULL, 0, NULL);
}



void CgiProcessor::setPidEvent(int pid)
{
	struct kevent ev;
	EV_SET(&ev, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, NULL);
	kevent(m_kq, &ev, 1, NULL, 0, NULL);
}

void CgiProcessor::inChild(Request &request, Server &server, std::vector<std::string> &cgi, std::string& filename, int p1[2], int p2[2])
{
	assert(&request);
	assert(&server);
	Location &location = server.GetLocationBlock(request.GetPath());
	std::string path = location.GetRoot() + request.GetPath();
	std::string query = request.GetQuery();
	std::string body = request.GetBody();
	std::string method = request.GetMethod();

	std::multimap<std::string, std::string> &header = request.GetHeaders();
	std::multimap<std::string, std::string>::iterator it = header.lower_bound("x");
	for (; it != header.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;
		for (size_t i = 0; i < key.length(); i++)
			key[i] = std::toupper(key[i]);
		std::cout << "extension header key is " << key << " and value is " << value << std::endl;
		addCgiEnv("HTTP_" + key, value);
	}
	addCgiEnv("REQUEST_METHOD", method);
	addCgiEnv("SCRIPT_NAME", path);
	addCgiEnv("CONTENT_TYPE", request.GetHeader("content-type"));
	addCgiEnv("CONTENT_LENGTH", inttoString(body.length()));
	addCgiEnv("GATEWAY_INTERFACE", "CGI/1.1");
	addCgiEnv("PATH_INFO", path);
	addCgiEnv("PATH_TRANSLATED", path);
	// std::cout << "path is " << path << std::endl;
	addCgiEnv("QUERY_STRING", query);
	addCgiEnv("SERVER_NAME", server.GetServerName()[0]);
	addCgiEnv("SERVER_PORT", inttoString(server.GetPort()[0]));
	addCgiEnv("SERVER_PROTOCOL", request.GetVersion());
	// addCgiEnv("SERVER_SOFTWARE", "webserv");






	// std::string contentLength = "CONTENT_LENGTH=" + std::to_string(body.length());
	// std::string queryString = "QUERY_STRING=" + query;
	// std::string requestMethod = "REQUEST_METHOD=" + method;
	// std::string scriptName = "SCRIPT_NAME=" + request.GetPath();
	// std::string scriptFilename = "SCRIPT_FILENAME=" + path;
	// std::string pathInfo = "PATH_INFO=" + request.GetPath();
	// std::string pathTranslated = "PATH_TRANSLATED=" + path;
	// std::string remoteAddr = "REMOTE_ADDR=" + request.GetHost();
	// std::string remotePort = "REMOTE_PORT=" + std::to_string(request.GetPort());
	// std::string serverName = "SERVER_NAME=" + server.GetServerName()[0];
	// std::string serverPort = "SERVER_PORT=" + std::to_string(server.GetPort()[0]);
	// std::string serverProtocol = "SERVER_PROTOCOL=" + request.GetVersion();
	// std::string serverSoftware = "SERVER_SOFTWARE=webserv";
	// std::cout << "contentLength : " << contentLength << std::endl;
	// std::cout << "queryString : " << queryString << std::endl;
	// std::cout << "requestMethod : " << requestMethod << std::endl;
	// std::cout << "scriptName : " << scriptName << std::endl;
	// std::cout << "scriptFilename : " << scriptFilename << std::endl;
	// std::cout << "pathInfo : " << pathInfo << std::endl;
	// std::cout << "pathTranslated : " << pathTranslated << std::endl;
	// std::cout << "remoteAddr : " << remoteAddr << std::endl;
	// std::cout << "remotePort : " << remotePort << std::endl;
	// std::cout << "serverName : " << serverName << std::endl;
	// std::cout << "serverPort : " << serverPort << std::endl;
	// std::cout << "serverProtocol : " << serverProtocol << std::endl;
	// std::cout << "serverSoftware : " << serverSoftware << std::endl;
	// char *envp[] = {
	// 	(char*)contentLength.c_str(),
	// 	(char*)queryString.c_str(),
	// 	(char*)requestMethod.c_str(),
	// 	(char*)scriptName.c_str(),
	// 	(char*)scriptFilename.c_str(),
	// 	(char*)pathInfo.c_str(),
	// 	(char*)pathTranslated.c_str(),
	// 	(char*)remoteAddr.c_str(),
	// 	(char*)remotePort.c_str(),
	// 	(char*)serverName.c_str(),
	// 	(char*)serverPort.c_str(),
	// 	(char*)serverProtocol.c_str(),
	// 	(char*)serverSoftware.c_str(),
	// 	NULL
	// };
	char *envp[m_envp.size() + 1];
	for (size_t i = 0; i < m_envp.size(); i++)
		envp[i] = (char*)m_envp[i].c_str();
	envp[m_envp.size()] = NULL;
	// dup2(p[0], STDIN_FILENO);
	// if (request.GetMethod() == "POST")
	// {
	// write(0, "\n", 1);
	close(p1[1]);
	close(p2[0]);
	dup2(p1[0], 0);
	dup2(p2[1], 1);
	close(p1[0]);
	close(p2[1]);
	char *argv[] = {
		(char *)cgi[1].c_str(),
		(char*)filename.c_str(),
		NULL
	};
	if (execve(cgi[1].c_str(), argv, envp) == -1)
	{
		perror("execve error");
		exit(99);
	}
}

bool CgiProcessor::isAllowedMethod(Request &request, Location &location)
{
	std::string method = request.GetMethod();
	std::vector<std::string> &methodSet = location.GetMethod();
	if (std::find(methodSet.begin(), methodSet.end(), method) == methodSet.end())
		return false;
	else
		return true;
}

void CgiProcessor::addCgiEnv(std::string key, std::string value)
{
	m_envp.push_back(key + "=" + value);
}

std::string CgiProcessor::inttoString(int num)
{
	std::string str;
	std::stringstream ss;
	ss << num;
	ss >> str;
	return str;
}

void CgiProcessor::parseCgiBody(Response& response, std::string& body)
{
	size_t start = 0;
	size_t pos;
	std::multimap<std::string, std::string> headers = response.GetHeaders();
	while ((pos = body.find("\r\n", start)) != std::string::npos)
	{
		std::string temp = body.substr(start, pos - start + 2);
		// std::cout << "temp is " << temp << std::endl;
		// for (size_t i = 0; i < temp.length(); i++)
		// {
		// 	printf("%d ", temp[i]);
		// }
		if (temp == "\r\n")
		{
			start = pos + 2;
			break ;
		}
		size_t subpos = temp.find(":");
		if (subpos == std::string::npos)
			break; 
		// \r\n파싱했는데 헤더가 아닐경우 바디 일부이므로 그냥 탈출
		std::string key = temp.substr(0, subpos);
		std::string value = temp.substr(subpos + 2, temp.length() - subpos - 4);
		std::cout << "key is " << key << " and value is " << value << std::endl;
		for (size_t i = 0; i < key.length(); i++)
			key[i] = std::tolower(key[i]);
		if (key == "status")
		{
			int status = std::stoi(value);
			response.SetStatusCode(status);
			response.SetStatusMessage(value);
		}
		else
			response.SetHeader(key, value);
		// std::cout << temp << std::endl;
		start = pos + 2;
	}
	if (start != 0)
	{
		if (start >= body.length())
			start--;
		body = body.substr(start);
		// std::cout << "parsed body is " << body << std::endl;
	}
}