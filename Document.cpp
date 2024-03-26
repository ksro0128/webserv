#include "Document.hpp"

Document::Document()
{

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

void Document::PutExcute(int pid, Request& req)
{
    if (m_excute.find(pid) != m_excute.end())
        m_excute[pid] = req;
    else
        m_excute.insert(std::pair<int, Request>(pid, req));
}
void Document::RemoveExcute(int pid)
{
    if (m_excute.find(pid) != m_excute.end())
        m_excute.erase(pid);
}
Request& Document::GetExcute(int pid)
{
    if (m_excute.find(pid) != m_excute.end())
        return m_excute[pid];
    else
        throw std::runtime_error("no such pid");
}

void Document::PutResponse(Response& res)
{
    m_response.push_back(res);
}

std::vector<Response>& Document::GetResponse()
{
    return m_response;
}