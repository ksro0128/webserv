#include "Document.hpp"

Document::Document()
{
    m_excuteCount = 0;
}
Document::~Document()
{

}
Document::Document(const Document& r)
{
    m_incomplete = r.m_incomplete;
    m_complete = r.m_complete;
    m_excute = r.m_excute;
}
Document& Document::operator=(const Document& r)
{
    if (this != &r)
    {
        m_incomplete = r.m_incomplete;
        m_complete = r.m_complete;
        m_excute = r.m_excute;
    }
    return *this;
}
void Document::PutIncomplete(int fd, Request& req)
{
    m_incomplete[fd] = req;
}
void Document::RemoveIncomplete(int fd)
{
    if (m_incomplete.find(fd) != m_incomplete.end())
        m_incomplete.erase(fd);
}
Request& Document::GetIncomplete(int fd)
{
    return m_incomplete[fd];
}
void Document::PutComplete(Request& req)
{
    m_complete.push_back(req);
}
void Document::RemoveComplete()
{
    m_complete.clear();
}
std::vector<Request>& Document::GetComplete()
{
	return m_complete;
}

void Document::PutExcute(int pipefd)
{
    m_excute[pipefd] = "";
}

void Document::RemoveExcute(int pipefd)
{
    if (m_excute.find(pipefd) != m_excute.end())
        m_excute.erase(pipefd);
}
std::string& Document::GetBuffer(int pipefd)
{
    if (m_excute.find(pipefd) != m_excute.end())
        return m_excute[pipefd];
    else
        throw std::runtime_error("no such pipefd");
}

std::map<int, std::string>& Document::GetExcute()
{
    return m_excute;
}

void Document::PutPidInfo(int pipefd, ExecInfo& info)
{
    m_pidInfo[pipefd] = info;
}

void Document::RemovePidInfo(int pipefd)
{
    if (m_pidInfo.find(pipefd) != m_pidInfo.end())
        m_pidInfo.erase(pipefd);
}

ExecInfo& Document::GetPidInfo(int pid)
{
    return m_pidInfo[pid];
}



void Document::PutResponse(Response& res)
{
    m_response.push_back(res);
}

std::vector<Response>& Document::GetResponse()
{
    return m_response;
}

int Document::GetExcuteCount()
{
    return m_excuteCount;
}

void Document::PutStatic(Request& req)
{
	m_static.push_back(req);
}

std::vector<Request>& Document::GetStatic()
{
	return m_static;
}

void Document::ClearStatic()
{
	m_static.clear();
}

void Document::PutDynamic(Request& req)
{
	m_dynamic.push_back(req);
}

std::vector<Request>& Document::GetDynamic()
{
	return m_dynamic;
}

void Document::ClearDynamic()
{
	m_dynamic.clear();
}

void Document::AddExcuteCount()
{
    m_excuteCount++;
}

void Document::SubExcuteCount()
{
    m_excuteCount--;
}


void Document::PutFdEvent(int fd, const std::string& identifier)
{
    m_fd_identifier.insert(std::pair<int, std::string>(fd, identifier));
}

std::multimap<int, std::string>& Document::GetFdEvent()
{
    return m_fd_identifier;
}

void Document::PutUploadFiles(int fd, UploadInfo& up)
{
    m_uploadFiles.insert(std::pair<int, UploadInfo>(fd, up));
}

std::map<int, UploadInfo>& Document::GetUploadFiles()
{
    return m_uploadFiles;
}