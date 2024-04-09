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
    ResponseInfo *info = new ResponseInfo;
    info->response = res;
    info->start = 0;
    info->rest = res.GetResponse().length();
    m_response.insert(std::pair<int, ResponseInfo*>(res.GetOriginFd(), info));
}

ResponseInfo* Document::GetResponse(int fd)
{
    return m_response[fd];
}

void Document::RemoveResponse(int fd)
{
    if (m_response.find(fd) != m_response.end())
    {
        delete m_response[fd];
        m_response.erase(fd);
    }
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

void Document::PutExcuteInfo(size_t hash, ExecInfo* info)
{
    m_excuteInfo[hash] = info;
}

void Document::RemoveExcuteInfo(size_t hash)
{
    if (m_excuteInfo.find(hash) != m_excuteInfo.end())
    {
        delete m_excuteInfo[hash];
        m_excuteInfo.erase(hash);
    }
}

ExecInfo* Document::GetExcuteInfoByHash(size_t hash)
{
    if (m_excuteInfo.find(hash) == m_excuteInfo.end())
        return (0);
    else
        return m_excuteInfo[hash];
}

void Document::PutPipeInfo(int pipefd, size_t hash)
{
    m_pipeInfo[pipefd] = hash;
}

void Document::RemovePipeInfo(int pipefd)
{
    if (m_pipeInfo.find(pipefd) != m_pipeInfo.end())
        m_pipeInfo.erase(pipefd);
}

size_t Document::GetHashByPipe(int pipefd)
{
    if (m_pipeInfo.find(pipefd) == m_pipeInfo.end())
        return 0;
    else
        return m_pipeInfo[pipefd];
}

size_t Document::GetHashByPid(int pid)
{
    if (m_pidInfo2.find(pid) == m_pidInfo2.end())
        return 0;
    else
        return m_pidInfo2[pid];
}

void Document::PutPidInfo2(int pid, size_t hash)
{
    m_pidInfo2[pid] = hash;
}

void Document::RemovePidInfo2(int pid)
{
    if (m_pidInfo2.find(pid) != m_pidInfo2.end())
        m_pidInfo2.erase(pid);
}

std::map<int, size_t>& Document::GetPipeInfo()
{
    return m_pipeInfo;
}

std::map<int, size_t>& Document::GetPidInfo2()
{
    return m_pidInfo2;
}

