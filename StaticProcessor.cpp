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
	Response response;
	Server& server = m_config.GetServer(request.GetPort(), request.GetHost());
	Location& location = server.GetLocationBlock(request.GetPath());
	if (request.GetStatus() != 200)
	{
		setResponse(request, response, request.GetStatus());
		putBody(response, server.GetErrorPage(request.GetStatus()));
		setEventWriteable(request.GetFd());
		document.PutResponse(response);
		return ;
	}
	if (isAllowedMethod(request, location) == false || (request.GetMethod() != "GET" && request.GetMethod() != "HEAD"))
	{
		setResponse(request, response, 405);
		putBody(response, server.GetErrorPage(405));
		putAllowedMethod(response, location); 
		setEventWriteable(request.GetFd());
		document.PutResponse(response);
		return ;
	}
	std::string path = getFilePath(document, request, response, server, location);
	if (path == "")
	{
		return ;
	}
	std::ifstream file(path.c_str());
	if (file.is_open() == false)
	{
		setResponse(request, response, 404);
		putBody(response, server.GetErrorPage(404));
		setEventWriteable(request.GetFd());
		document.PutResponse(response);
		return ;
	}
	setResponse(request, response, 200);
	putBody(response, path);
	setEventWriteable(request.GetFd());
	document.PutResponse(response);
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

void StaticProcessor::setResponse(Request &request, Response &response, int status)
{
	response.SetVersion("HTTP/1.1");
	response.SetStatusCode(status);
	response.SetStatusMessage(m_config.GetStatusMessage(status));
	response.SetOriginFd(request.GetFd());
}


void StaticProcessor::putBody(Response& response, std::string path)
{
	std::ifstream file(path.c_str());
	std::ostringstream oss;
	oss << file.rdbuf();
	response.SetBody(oss.str());
	response.SetHeader("Content-type", m_config.GetMimeType(m_config.GetExtension(path)));
}

void StaticProcessor::putAllowedMethod(Response& response, Location& location)
{
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
}

void StaticProcessor::setEventWriteable(int fd)
{
	struct kevent ev;
	EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, 0);
	kevent(m_kq, &ev, 1, NULL, 0, NULL);
}

std::string StaticProcessor::getFilePath(Document& document, Request &request, Response &response, Server& server, Location& location)
{
	std::string filePath = location.GetRoot() + request.GetPath();
	std::cout << "root : " << location.GetRoot() << std::endl;
	std::cout << "path : " << request.GetPath() << std::endl;
	std::cout << "filePath : " << filePath << std::endl;
	location.PrintLocation();
	struct stat buf;
	if (stat(filePath.c_str(), &buf) == -1)
	{
		if (errno == EACCES)
		{
			setResponse(request, response, 403);
			putBody(response, server.GetErrorPage(403));
			setEventWriteable(request.GetFd());
			document.PutResponse(response);
			return "";
		}
		else
		{
			setResponse(request, response, 404);
			putBody(response, server.GetErrorPage(404));
			setEventWriteable(request.GetFd());
			document.PutResponse(response);
			return "";
		}
	}
	if (S_ISDIR(buf.st_mode) && filePath[filePath.size() - 1] != '/')
	{
		filePath += "/";
		setResponse(request, response, 301);
		putBody(response, server.GetErrorPage(301));
		response.SetHeader("Location", filePath);
		setEventWriteable(request.GetFd());
		document.PutResponse(response);
		return "";
	}
	if (S_ISDIR(buf.st_mode))
	{
		std::vector<std::string> &index = location.GetIndex();
		if (index.size() == 0)
		{
			if (location.GetAutoIndex() == false)
			{
				setResponse(request, response, 403);
				putBody(response, server.GetErrorPage(403));
				setEventWriteable(request.GetFd());
				document.PutResponse(response);
				return "";
			}
			else
			{
				// autoindex
				setResponse(request, response, 200);

				//영찬 autoindex
				setEventWriteable(request.GetFd());
				document.PutResponse(response);
				return "";
			}
		}
		for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); it++)
		{
			std::string indexPath = filePath + *it;
			std::cout << "indexPath : " << indexPath << std::endl;
			std::fstream file(indexPath.c_str());
			if (file.is_open())
			{
				filePath = indexPath;
				break;
			}
		}
		if (filePath == location.GetRoot() + request.GetPath())
		{
			if (location.GetAutoIndex() == false)
			{
				setResponse(request, response, 403);
				putBody(response, server.GetErrorPage(403));
				setEventWriteable(request.GetFd());
				document.PutResponse(response);
				return "";
			}
			else
			{
				// autoindex
				setResponse(request, response, 200);

				//영찬 autoindex
				setEventWriteable(request.GetFd());
				document.PutResponse(response);
				return "";
			}
		}
	}
	return filePath;
}