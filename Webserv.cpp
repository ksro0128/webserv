#include "Webserv.hpp"

Webserv::Webserv(std::string path, char** envp)
{
    //컨피그 만들기
	mConfig.ParseConfig(path);
	for (int i = 0; envp[i]; i++)
		mEnv.push_back(envp[i]);
	mKq = kqueue();
	if (mKq == -1)
		throw std::runtime_error("kqueue error");
	std::vector<Server> servSocket;
	mPorts = mConfig.GetPorts();

	struct kevent ev_set;
	for (std::vector<int>::iterator it = mPorts.begin(); it != mPorts.end(); it++)
	{
		mServSocks.push_back(openPort(*it));
		EV_SET(&ev_set, mServSocks.back(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		kevent(mKq, &ev_set, 1, NULL, 0, NULL);
		mType[mServSocks.back()] = SERVER;
		mFid[SERVER][mServSocks.back()] = mServSocks.back();
	}
	signal(SIGPIPE, SIG_IGN);
    //컨피그로 서버포트 열기
    //서버fd 를 플래그 맵에 저장
    //kq에 서버fd 리드 이벤트 등록
}

Webserv::~Webserv()
{
    //dtor
}

void Webserv::Run()
{
    struct kevent ev_list[10000];
    while (1)
    {
        int nev = kevent(mKq, NULL, 0, ev_list, 10000, NULL);
        if (nev == -1)
        {
            std::cout << "kevent error" << std::endl;
            return ; // 일단은 kevent 에러 시 종료
        }
        for (int i = 0; i < nev; i++)
        {
            int ident = ev_list[i].ident;
            int type = mType[ident];
			if (type == 0)
				continue;
            Doc* document = NULL;
            if (type != SERVER)
                document = mDoc[mFid[type][ident]];
			if (ev_list[i].filter == EVFILT_TIMER)
			{
				if (document == 0) // 애초에 클로즈되어서 이미 없어진 놈이라면 혹시모르니까
				{
					std::cout << "Timer : document is already closed" << std::endl;
					continue;
				}
				if (!document->GetRequest()) // 연결은됐는데 리퀘스트조차 안들어온놈이라면 바로 그냥 연결종료시키기.
				{
					std::cout << "Timer : request is not arrived" << std::endl;
					close(ident);
					mType.erase(ident);
					mFid[CLIENT].erase(ident);
					delete document;
					continue;
				}
				if (document->GetCgiFlag()) // cgi가 실행중인 놈이라면 cgi관련 다 닫고 리스폰스 만들기.
				{
					std::cout << "Timer : cgi is running" << std::endl;
					deleteCgiInfo(document);
					makeResponse(document, 408, mConfig.GetStatusMessage(408), "");
					continue;
				}
				if (!document->GetStatus()) // 리퀘스트는 있는데 cgi도 아니고 리스폰스도 안만들어진놈이라면
				{
					std::cout << "Timer : request is not processed" << std::endl;
					close(ident);
					mType.erase(ident);
					mFid[CLIENT].erase(ident);
					delete document;
					continue;
				}
				if (document->GetResponse())   // 리스폰스가 만들어진 놈이라면 그냥 보내버리기.
				{
					std::cout << "Timer : response is already made so ignored" << std::endl;
					continue;
				}
				// 만약 클라이언트가 헤더조차 덜보내서 리스폰스를 만들 준비조차 안되는 건 당연하고
				// 현재 코드상 리스폰스조차 작성하기에 정보가 부족할 경우 어떻게 처리할것인가
			}
            if (ev_list[i].filter == EVFILT_READ) // read 이벤트
            {
                switch(type)
                {
                    case SERVER:
                        registerClient(ident, document);
                        break;
                    case CLIENT:
                        // std::cout << "client read" << std::endl;
                        // if (document->GetStatus() > 0) // busy
                        //     continue;
						if (document->GetStatus() == 1)
							continue;
						setTimer(ident, 30, EV_ADD);
                        if (parseRequest(ident, document))
						{
							delete document;
							mDoc.erase(ident);
							continue;
						}
                        processRequest(document);
                        break;
                    case PIPE:
						// std::cout << "read pipe\n";
                        readPipe(ident, document);
                        // pipe 읽기
                }
            }
            else if (ev_list[i].filter == EVFILT_WRITE) // write 이벤트
            {
                switch(type)
                {
                    case CLIENT:
                        sendResponse(ident, document);
                        break;
                    case PIPE:
						// std::cout << "write pipe\n";
                        writePipe(ident, document);
                        // pipe 쓰기
                }
            }
            else if (ev_list[i].filter == EVFILT_PROC) // process 이벤트
			{
				// std::cout << "process event\n";
                waitProcess(ident, document);
			}
        }
    }
}

int Webserv::parseRequest(int fd, Doc* document)
{
    if (!document->GetRequest())
        document->SetRequest(new Request());
	if (readParse(fd, document->GetRequest()) == -1)
    {
        document->RemoveRequest();
		if (document->GetCgiFlag() == 1)
			deleteCgiInfo(document);
        mFid[CLIENT].erase(fd);
        mType.erase(fd);
		close(fd);
        // std::cout << "while parsing, document is removed" << std::endl;
        return (1);
    }
	// if (document->GetRequest()->IsComplete() == true)
    // 	document->GetRequest()->PrintRequest();
	return (0);
}

int Webserv::readParse(int fd, Request* request)
{
    char buf[1025];
    int len = read(fd, buf, 1024);
    if (len == -1)
    {
        std::cout << "read return -1 in read client, so close socket" << std::endl;
        close(fd);
        return -1;
    }
    else if (len == 0)
    {
        // std::cout << "write return 0 so client close socket" << std::endl;
        close(fd);
        return -1;
    }
    buf[len] = '\0';
    std::string buffer(buf, len);
    request->ParseRequest(fd, buffer);
    return (0);
}

void Webserv::excuteCGI(Doc* document)
{
    Request* request = document->GetRequest();
    std::string tmppath = request->GetPath();
    int port = request->GetPort();
    std::string host = request->GetHost();
    std::string path = mConfig.GetFilePath(port, host, tmppath);
    std::string cgi_path = mConfig.GetCgiPath(request->GetPort(), request->GetHost(), path);
    int origin_fd = request->GetFd();
    // std::cout << "in cgi path is " << path << std::endl;
    // std::cout << "extension is " << extension << std::endl;
    // std::cout << "cgi path is " << cgi_path << std::endl;
    // return ;
	// std::cout << "in cgi, path is " << path << std::endl;
    if (access(cgi_path.c_str(), F_OK) == -1 || access(cgi_path.c_str(), X_OK) == -1)
    {
        setResponseError(document, 502);
        setEvent(request->GetFd(), EVFILT_WRITE, EV_ADD, 0);
        std::cout << "Cgi path error" << std::endl;
        return;
    }
    int p1[2];
    int p2[2];
    if (pipe(p1) == -1 || pipe(p2) == -1)
    {
        makeResponse(document, 500, mConfig.GetStatusMessage(500), "");
        setEvent(request->GetFd(), EVFILT_WRITE, EV_ADD, 0);
        return;
    }
    int pid = fork();
    if (pid == -1)
    {
        makeResponse(document, 500, mConfig.GetStatusMessage(500), "");
        setEvent(request->GetFd(), EVFILT_WRITE, EV_ADD, 0);
        return;
    }
    if (pid == 0)
    {
        inChild(document, path, cgi_path, p1, p2);
    }
    else
    {
        close(p1[0]);
        close(p2[1]);
        fcntl(p2[0], F_SETFL, O_NONBLOCK);
        fcntl(p1[1], F_SETFL, O_NONBLOCK);
        setEvent(p2[0], EVFILT_READ, EV_ADD, 0);
        setEvent(pid, EVFILT_PROC, EV_ADD, NOTE_EXIT);
        mType[p2[0]] = PIPE;
        mFid[PIPE][p2[0]] = origin_fd;
        mType[pid] = PID;
        mFid[PID][pid] = origin_fd;
        if (request->GetBody().length() > 0)
        {
            // std::cout << "there is body to write cgi and write pipe is " << p1[1] << std::endl;
            document->SetExecInfo(p2[0], p1[1], pid);
            document->SetIndexInfo(0, request->GetBody().length());
            mType[p1[1]] = PIPE;
            mFid[PIPE][p1[1]] = origin_fd;
            setEvent(p1[1], EVFILT_WRITE, EV_ADD, 0);
        }
        else
        {
            close(p1[1]);
            document->SetExecInfo(p2[0], -1, pid);
			document->SetIndexInfo(0, 0);
        }
        // std::cout << "excute cgi! client fd is" << request->GetFd() << " and write pipe is" << p1[1] << " read pipe is " << p2[0] << std::endl;
    }
}

void Webserv::addEnv(const std::string& key, const std::string& value)
{
    mEnv.push_back(key + "=" + value);
}

std::string Webserv::intToString(int num)
{
    std::string str;
    std::stringstream ss;
    ss << num;
    ss >> str;
    return str;
}

void Webserv::inChild(Doc* document, std::string& path, std::string& cgi_path, int p1[2], int p2[2])
{
    Request* request = document->GetRequest();
	std::string query = request->GetQuery();
	std::string body = request->GetBody();
	std::string method = request->GetMethod();
	std::multimap<std::string, std::string> &header = request->GetHeaders();
	std::multimap<std::string, std::string>::iterator it = header.lower_bound("x");
	for (; it != header.end(); it++)
	{
		std::string key = it->first;
		std::string value = it->second;
		for (size_t i = 0; i < key.length(); i++)
			key[i] = std::toupper(key[i]);
		// std::cout << "extension header key is " << key << " and value is " << value << std::endl;
		addEnv("HTTP_" + key, value);
	}
	addEnv("REQUEST_METHOD", method);
	addEnv("CONTENT_TYPE", request->GetHeader("content-type"));
	addEnv("CONTENT_LENGTH", intToString(body.length()));
	addEnv("GATEWAY_INTERFACE", "CGI/1.1");
	addEnv("PATH_INFO", path);
	addEnv("QUERY_STRING", query);
	addEnv("SERVER_PORT", intToString(request->GetPort()));
	addEnv("SERVER_PROTOCOL", request->GetVersion());
	char *envp[mEnv.size() + 1];
	for (size_t i = 0; i < mEnv.size(); i++)
		envp[i] = (char*)mEnv[i].c_str();
	envp[mEnv.size()] = NULL;
	close(p1[1]);
	close(p2[0]);
	if (dup2(p1[0], 0) == -1 || dup2(p2[1], 1) == -1)
	{
		close(p1[0]);
		close(p2[1]);
		std::exit(99);
	}
	close(p1[0]);
	close(p2[1]);
	char *argv[] = {
		(char *)cgi_path.c_str(),
		(char*)path.c_str(),
		NULL
	};
	if (execve(cgi_path.c_str(), argv, envp) == -1)
	{
		perror("execve error");
		std::exit(99);
	}
}

void Webserv::readPipe(int fd, Doc* document)
{
    char buf[65537];
    int len = read(fd, buf, 65536);
    if (len == -1)
    {
        // std::cout << "cannot read buffer is full" << std::endl;
        return ;
    }
    if (len == 0)
    {
        // std::cout << "read pipe closed because write pipe already closed" << std::endl;
        close(fd);
        mFid[PIPE].erase(fd);
        mType.erase(fd);
        return ;
    }
    buf[len] = '\0';
    // std::string tmp(buf, len);
    // std::cout << "read pipe is " << fd << " and read len is " << len << std::endl;
    document->GetBuffer() += buf;
}

void Webserv::writePipe(int fd, Doc* document)
{
    Request* request = document->GetRequest();
    std::string& body = request->GetBody();
    IndexInfo& indexInfo = document->GetIndexInfo();
    int& start = indexInfo.start;
    int& rest = indexInfo.rest;
	// std::cout << "start is " << start << " and rest is " << rest << std::endl;
    int len;
    if (rest > 0)
    {
        if (rest > 65535)
            len = write(fd, &body[start], 65535);
        else
            len = write(fd, &body[start], rest);
        if (len == -1)
            return ;
        else if (len == 0)
        {
            // std::cout << "write pipe closed because read pipe already closed" << std::endl;
            close(fd);
            mFid[PIPE].erase(fd);
            mType.erase(fd);
            return ;
        }
        else
        {
            start += len;
            rest -= len;
        }
    }
    else
    {
        // std::cout << "there is no more body to write\n";
        close(fd);
        mFid[PIPE].erase(fd);
        mType.erase(fd);
    }
}

void Webserv::waitProcess(int pid, Doc* document)
{
    int status;
    waitpid(pid, &status, 0);
    mFid[PID].erase(pid);
    mType.erase(pid);
    if (WIFEXITED(status))
    {
		if (WEXITSTATUS(status) != 0)
		{
			std::string& body = document->GetBuffer();
			if (body.find("Could not open input file") != std::string::npos || \
				body.find("can't open file") != std::string::npos)
				makeResponse(document, 404, mConfig.GetStatusMessage(404), "");
			else
				makeResponse(document, 500, mConfig.GetStatusMessage(500), "");
			setEvent(document->GetRequest()->GetFd(), EVFILT_WRITE, EV_ADD, 0);
			document->SetExecInfo(-1, -1, -1);
			return;
		}
        std::string& buffer = document->GetBuffer();
        setEvent(document->GetRequest()->GetFd(), EVFILT_WRITE, EV_ADD, 0);
        makeResponse(document, 200, mConfig.GetStatusMessage(200), buffer);
        parseCgiBody(document, buffer);
		document->SetExecInfo(-1, -1, -1);
    }
    else if (WIFSIGNALED(status))
    {
        std::cout << "child process killed by signal " << WTERMSIG(status) << std::endl;
		return ;
        // signal kill에 따른 처리
    }
    else
    {
        makeResponse(document, 500, mConfig.GetStatusMessage(500), "");
        setEvent(document->GetRequest()->GetFd(), EVFILT_WRITE, EV_ADD, 0);
		document->SetExecInfo(-1, -1, -1);
    }
}

void Webserv::parseCgiBody(Doc* document, std::string& body)
{
    size_t start = 0;
	size_t pos;
    Response* response = document->GetResponse();
	std::multimap<std::string, std::string>& headers = response->GetHeaders();
	while ((pos = body.find("\r\n", start)) != std::string::npos)
	{
		std::string temp = body.substr(start, pos - start + 2);
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
		// std::cout << "key : " << key << std::endl;
		// std::cout <<  "value : " << value << std::endl;
		for (size_t i = 0; i < key.length(); i++)
			key[i] = std::tolower(key[i]);
		if (key == "status")
		{
			std::istringstream iss(value);
			std::string num, msg;
			iss >> num;
			iss >> msg;
			int status = std::atoi(num.c_str());
			response->SetStatusCode(status);
			response->SetStatusMessage(msg);
		}
		else
        {
            if (headers.find(key) != headers.end())
                headers.erase(headers.find(key));
			response->SetHeader(key, value);
        }
		start = pos + 2;
	}
	if (start != 0)
	{
		if (start >= body.length())
			start--;
		body = body.substr(start);
		response->SetBody(body);
		// std::cout << "parsed body is " << body << std::endl;
	}
}

void Webserv::setEvent(int fd, int filter, int flag, int fflag)
{
    struct kevent ev;
    EV_SET(&ev, fd, filter, flag, fflag, 0, NULL);
    kevent(mKq, &ev, 1, NULL, 0, NULL);
}

int Webserv::openPort(int port)
{
	int servSock;
	struct sockaddr_in servAddr;
	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == -1)
		throw std::runtime_error("socket error");
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);
	if (fcntl(servSock, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl error");
	if (bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
		throw std::runtime_error("bind error");
	if (listen(servSock, 1000) == -1)
		throw std::runtime_error("listen error");
	return servSock;
}

void Webserv::setResponseError(Doc* document, int status)
{
    Request* request = document->GetRequest();
    Response *response = new Response();
	response->SetVersion("HTTP/1.1");
	response->SetStatusCode(status);
    response->SetStatusMessage(mConfig.GetStatusMessage(status));
	response->SetOriginFd(request->GetFd());
	response->SetHeader("Content-Type", "text/html");
    response->SetBody(mConfig.GetErrorPage(request->GetPort(), request->GetHost(), status));
    document->SetResponse(response);
}

void Webserv::registerClient(int fd, Doc* document)
{
    struct sockaddr_in clnt_adr;
    socklen_t adr_sz = sizeof(clnt_adr);
    int clntSock = accept(fd, (struct sockaddr *)&clnt_adr, &adr_sz);
    fcntl(clntSock, F_SETFL, O_NONBLOCK);
    struct linger {
            int l_onoff;
            int l_linger;
    };
    struct linger _linger;
    _linger.l_onoff = 1;
    _linger.l_linger = 0;
    setsockopt(clntSock, SOL_SOCKET, SO_LINGER, &_linger, sizeof(_linger));
    setEvent(clntSock, EVFILT_READ, EV_ADD, 0);
    mType[clntSock] = CLIENT;   // 클라이언트 타입으로 등록
    mFid[CLIENT][clntSock] = clntSock; // 연관맵에 클라이언트 소켓 등록
    document = new Doc(); // 도큐먼트 생성
    document->SetFd(clntSock); // 도큐먼트에 클라이언트 소켓 등록
    mDoc[clntSock] = document; // 도큐먼트 맵에 등록
	setTimer(clntSock, 30, EV_ADD);
    std::cout << "new client fd is " << clntSock << std::endl;
}

void Webserv::processRequest(Doc* document)
{
	if (checkBodySize(document) == 1)
		return ;
    if (document->GetRequest()->GetReqClose() == 0 && document->GetRequest()->IsComplete() == false)
        return ;
	document->SetStatus(1);
	if (checkRequestError(document) == 1)
		return ;
	if (checkMethod(document) == 1)
		return ;
	if (checkRedirection(document) == 1)
		return ;
	if (checkCgi(document) == 1)
		return ;
	if (checkIndex(document) == 1)
		return ;
	processStatic(document);
}

void Webserv::processStatic(Doc* document)
{
	Request* request = document->GetRequest();
	std::string method = request->GetMethod();
	if (method == "GET")
		processGet(document);
	else if (method == "POST")
		processPost(document);
	else if (method == "DELETE")
		processDelete(document);
}

int Webserv::checkBodySize(Doc* document)
{
	int length = document->GetRequest()->GetBody().length();
	if (length > 0)
	{
		int body_limit = mConfig.GetLimitBodySize(document->GetRequest()->GetPort(), document->GetRequest()->GetHost(), document->GetRequest()->GetPath());
		if (body_limit != -1 && length > body_limit)
		{
			document->SetStatus(1);
			makeResponse(document, 413, mConfig.GetStatusMessage(413), "");
			return (1);
		}
	}
	return (0);
}

int Webserv::checkRequestError(Doc* document)
{
	Request* request = document->GetRequest();
	if (request->GetStatus() != 200) // 파싱 에러일때
	{
		makeResponse(document, request->GetStatus(), mConfig.GetStatusMessage(request->GetStatus()), "");
		return (1);
	}
	return (0);
}

int Webserv::checkMethod(Doc* document)
{
	Request* request = document->GetRequest();
	std::string host = request->GetHost();
	std::string path = request->GetPath();
	int port = request->GetPort();
	std::string method = request->GetMethod();
	std::vector<std::string>& allow_method = mConfig.GetMethods(port, host, path);
	if (std::find(allow_method.begin(), allow_method.end(), method) == allow_method.end())
	{
		std::string allow;
		for (size_t i = 0; i < allow_method.size(); i++)
		{
			allow = allow + allow_method[i] + ", ";
		}
		allow = allow.substr(0, allow.length() - 2);
		makeResponse(document, 405, mConfig.GetStatusMessage(405), "");
		document->GetResponse()->SetHeader("Allow", allow);
		return (1);
	}
	return (0);
}

int Webserv::checkRedirection(Doc* document)
{
	Request* request = document->GetRequest();
	std::string host = request->GetHost();
	std::string path = request->GetPath();
	int port = request->GetPort();
	std::string redirection = mConfig.GetRedirectionPath(port, host, path);
	if (redirection.length() > 0)
	{
		int status = mConfig.GetRedirectStatus(port, host, path);
		makeResponse(document, status, mConfig.GetStatusMessage(status), "");
		document->GetResponse()->SetHeader("Location", redirection);
		return 1;
	}
	return 0;
}

int Webserv::checkIndex(Doc* document)
{
	Request* request = document->GetRequest();
	std::string host = request->GetHost();
	std::string path = request->GetPath();
	int port = request->GetPort();
	std::string method = request->GetMethod();
	std::string filepath = mConfig.GetFilePath(port, host, path);
	// std::cout << "in check index, file path is " << filepath << std::endl;
	struct stat buf;
	if (stat(filepath.c_str(), &buf) == -1)
	{
		if (errno == EACCES)
		{
			makeResponse(document, 403, mConfig.GetStatusMessage(403), "");
			return (1);
		}
		else
		{
			if (method == "POST")
			{
                processPost(document);
				return (1);
			}
			else
			{
				makeResponse(document, 404, mConfig.GetStatusMessage(404), "");
				return (1);
			}
		}
	}
	if (S_ISDIR(buf.st_mode) && filepath[filepath.length() - 1] != '/')
	{
		std::string tmp = request->GetPath() + "/";
		makeResponse(document, 301, mConfig.GetStatusMessage(301), "");
		document->GetResponse()->SetHeader("Location", tmp);
		return (1);
	}
	else if (S_ISDIR(buf.st_mode))
	{
		std::vector<std::string>& indexes = mConfig.GetIndexes(port, host, path);
		if (indexes.size() == 0)
		{
			if (mConfig.IsAutoIndex(port, host, path) == false)
			{
				makeResponse(document, 404, mConfig.GetStatusMessage(404), "");
				return (1);
			}
			else
			{
				makeResponse(document, 200, mConfig.GetStatusMessage(200), makeAutoIndex(filepath));
				return (1);
			}
		}
		else
		{
			for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++)
			{
				std::string indexpath = filepath + *it;
				// std::cout << "index path is " << indexpath << std::endl;
				std::fstream file(indexpath.c_str());
				if (file.is_open())
				{
					if (mConfig.GetCgiPath(port, host, indexpath).length() > 0)
					{
                        std::string newpath = request->GetPath() + "/" + *it;
						request->SetPath(newpath);
						excuteCGI(document);
						return (1);
					}
					else
					{
                        std::string newpath = request->GetPath() + "/" + *it;
						request->SetPath(newpath);
						processStatic(document);
						return (1);
					}
				}
			}
			makeResponse(document, 404, mConfig.GetStatusMessage(404), "");
			return (1);
		}
	}
	return 0;
}

int Webserv::checkCgi(Doc* document)
{
	Request* request = document->GetRequest();
	std::string host = request->GetHost();
	std::string path = request->GetPath();
	int port = request->GetPort();
	std::string method = request->GetMethod();
	std::string filepath = mConfig.GetFilePath(port, host, path);
	if (mConfig.GetCgiPath(port, host, filepath).length() > 0)
	{
		excuteCGI(document);
		return 1;
	}
	return 0;
}

void Webserv::processGet(Doc* document)
{
	Request* request = document->GetRequest();
	std::string host = request->GetHost();
	std::string path = request->GetPath();
	int port = request->GetPort();
	std::string filepath = mConfig.GetFilePath(port, host, path);
    // std::cout << "filepath is " << filepath << std::endl;
	std::ifstream file(filepath.c_str());
    if (file.is_open() == false)
    {
        makeResponse(document, 404, mConfig.GetStatusMessage(404), "");
        return ;
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    // std::cout << "file is " << oss.str() << std::endl;
    makeResponse(document, 200, mConfig.GetStatusMessage(200), oss.str());
	document->GetResponse()->RenewHeader("Content-Type", mConfig.GetMimeType(mConfig.GetExtension(request->GetPath())));
}

void Webserv::processPost(Doc* document)
{
	Request* request = document->GetRequest();
	std::string host = request->GetHost();
	std::string path = request->GetPath();
	// std::cout << "path is " << path << std::endl;

	if (path[path.length() - 1] == '/')
	{
		makeResponse(document, 404, mConfig.GetStatusMessage(404), "");
		return ;
	}
	int port = request->GetPort();
	std::string filepath = mConfig.GetFilePath(port, host, path);
	// std::cout << "file path is " << filepath << std::endl;
	if (filepath[filepath.length() - 1] == '/')
		filepath = filepath.substr(0, filepath.length() - 1);
	// std::cout << "in post, file path is " << filepath << std::endl;
	// struct stat buf;
	// if (stat(filepath.c_str(), &buf) == -1)
	// {
		std::ofstream file(filepath.c_str(), std::ios::out);
		if (file.is_open() == false)
		{
			makeResponse(document, 404, mConfig.GetStatusMessage(404), "");
			return ;
		}
		file << request->GetBody();
		file.close();
		makeResponse(document, 201, mConfig.GetStatusMessage(201), "");
	// }
	// else
	// {
	// 	std::vector<std::string> &allowed_methods = mConfig.GetMethods(port, host, path);
	// 	std::string allow = "";
	// 	for (size_t i = 0; i < allowed_methods.size(); i++)
	// 	{
	// 		if (allowed_methods[i] == "GET" || allowed_methods[i] == "HEAD")
	// 			allow += allowed_methods[i] + ", ";
	// 	}
	// 	if (allow != "")
	// 		allow = allow.substr(0, allow.length() - 2);
	// 	makeResponse(document, 405, mConfig.GetStatusMessage(405), "");
	// 	document->GetResponse()->SetHeader("Allow", allow);
	// }
}

void Webserv::processDelete(Doc* document)
{
	Request* request = document->GetRequest();
	std::string host = request->GetHost();
	std::string path = request->GetPath();
	int port = request->GetPort();
	std::string filepath = mConfig.GetFilePath(port, host, path);
	struct stat buf;
	if (stat(filepath.c_str(), &buf) == -1)
	{
		if (errno == EACCES)
		{
			makeResponse(document, 403, mConfig.GetStatusMessage(403), "");
			return ;
		}
		else
		{
			makeResponse(document, 404, mConfig.GetStatusMessage(404), "");
			return ;
		}
	}
	if (std::remove(filepath.c_str()) == -1)
	{
		makeResponse(document, 500, mConfig.GetStatusMessage(500), "");
		return ;
	}
	makeResponse(document, 200, mConfig.GetStatusMessage(200), "");
}

void Webserv::makeResponse(Doc* document, int status, const std::string& msg, const std::string& body)
{
    Request* request = document->GetRequest();
	Response *response = document->GetResponse();
	if (response == NULL)
    	response = new Response();
    response->SetVersion("HTTP/1.1");
    response->SetStatusCode(status);
    response->SetStatusMessage(msg);
    response->SetOriginFd(request->GetFd());
    std::string mimetype = mConfig.GetMimeType(mConfig.GetExtension(request->GetPath()));
    // std::cout << "mimetype is " << mimetype << std::endl;
    if (mimetype != "application/octet-stream" && mimetype != "")
        response->SetHeader("Content-Type", mimetype);
    else
        response->SetHeader("Content-Type", "text/html");
    if (mConfig.GetErrorPage(request->GetPort(), request->GetHost(), status).length() > 0 && status != 200)
    // 200 제외하고 에러페이지 있으면 그걸로 바디설정, 200은 바디 들어올테니. 안들어와도 뭐. 수로한테확인. 이렇게 처리한 가장큰 이유는 head는 바디가 안들어가야되니까?
    {
        response->SetBody(mConfig.GetErrorPage(request->GetPort(), request->GetHost(), status));
    }
    else
        response->SetBody(body);
    // response->SetHeader("Content-Length", std::to_string(response->GetBody().length()));
    if (request->GetHeader("connection").length() > 0)
		response->SetHeader("Connection", request->GetHeader("connection"));
    document->SetResponse(response);
    setEvent(request->GetFd(), EVFILT_WRITE, EV_ADD, 0);
}

std::string Webserv::makeAutoIndex(const std::string& filepath)
{
	std::string body = "<html><head><title>Index of " + filepath + "</title></head><body><h1>Index of " + filepath + "</h1><hr><pre>";
	std::string whitespace;
	DIR *dp;
	struct dirent *dir;
	struct stat st;
	struct tm *nowtm;
	char tmbuf[64];
    timespec  date;
	std::string before = "<a href=\"";
	std::string after = "\">";
	std::string	size_str;
	int	flag = 0;
	long	size;
	if ((dp = opendir(filepath.c_str())) != NULL)
	{
		while ((dir = readdir(dp)) != NULL)
    	{
			whitespace = "";
			if (dir->d_name[0] == '.' && dir->d_name[1] == '\0')
				continue;
			std::string name = dir->d_name;
			stat((filepath + name).c_str(), &st);
			if (S_ISDIR(st.st_mode))
			{
				date = st.st_mtimespec;
				int len = name.length();
				for (int i = 30; i > len; i--)
					whitespace += " ";
				body += before + name + after + name + "</a>" + whitespace + '-' + "\n";
			}
    	}
		closedir(dp);
		flag++;
	}
	if ((dp = opendir(filepath.c_str())) != NULL)
	{
		while ((dir = readdir(dp)) != NULL)
		{
			whitespace = "";
			if (dir->d_name[0] == '.')
				continue;
			std::string name = dir->d_name;
			stat((filepath + name).c_str(), &st);
			if (!S_ISDIR(st.st_mode))
			{
				date = st.st_mtimespec;
				int len = name.length();
				for (int i = 30; i > len; i--)
					whitespace += " ";
				nowtm = localtime(&date.tv_sec);
				strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
				std::string date_str = tmbuf;
				size = st.st_size;
				if (size > 1024)
					size_str = std::to_string(size / 1024) + "K";
				else
					size_str = std::to_string(size) + "B";
				body += before + name + after + name + "</a>" + whitespace + size_str + "\t" + tmbuf + "\n";
			}
		}
		flag++;
		closedir(dp);
	}
	body += "</pre><hr></body></html>";
	if (flag == 2)
		return body;
	else
		return "";
}

void Webserv::sendResponse(int fd, Doc* document)
{
	Response* response = document->GetResponse();
	int len;
	std::string& responseStr = response->GetResponse();
	const char *buf = responseStr.c_str();
    int start = response->GetStartIndex();
    int rest = response->GetRestIndex();
	if (rest > 0)
	{
		if (rest > 200000)
			len = write(fd, &buf[start], 200000);
		else
			len = write(fd, &buf[start], rest);
		if (len == -1)
		{
			// std::cout << "write return -1 or 0 in fd" << fd << std::endl;
			close(fd);
			mFid[CLIENT].erase(fd);
			mType.erase(fd);
			delete document;
			mDoc.erase(fd);
			return;
		}
		else if (len == 0)
			response->RenewIndex(len);
		else
			response->RenewIndex(len);
		if (response->GetRestIndex() != 0)
			return ;
	}
    // std::cout << "complete write to client " << fd << "\n";
	// response->PrintResponse();
	document->GetBuffer().clear();
	document->SetStatus(0);
	if (response->GetHeader("Connection") == "close" \
		|| response->GetStatusCode() == 408 || \
		  document->GetRequest()->GetReqClose() == 1)
	{
		close(fd);
        mFid[CLIENT].erase(fd);
        mType.erase(fd);
		delete document;
		mDoc.erase(fd);
		std::cout << "Connection closed fd " << fd << "\n";
		setTimer(fd, 0, EV_DELETE);
		return;
	}
    document->RemoveResponse();
    document->RemoveRequest();
    setEvent(fd, EVFILT_WRITE, EV_DELETE, 0);
}

void Webserv::setTimer(int fd, int time, int flag)
{
	struct kevent ev;
	EV_SET(&ev, fd, EVFILT_TIMER, flag | EV_ONESHOT, NOTE_SECONDS, time, NULL);
	kevent(mKq, &ev, 1, NULL, 0, NULL);
}

void Webserv::deleteCgiInfo(Doc *document)
{
	int readpipe = document->GetReadPipe();
	int writepipe = document->GetWritePipe();
	int pid = document->GetPid();
	close(readpipe);
	if (writepipe != -1)
		close(writepipe);
	setEvent(pid, EVFILT_PROC, EV_DELETE, 0);
	kill(pid, SIGKILL);
	waitpid(pid, NULL, 0);
	document->SetExecInfo(-1, -1, -1);
	mType.erase(readpipe);
	if (writepipe != -1)
		mType.erase(writepipe);
	mType.erase(pid);
	mFid[PIPE].erase(readpipe);
	if (writepipe != -1)
		mFid[PIPE].erase(writepipe);
	mFid[PID].erase(pid);
}