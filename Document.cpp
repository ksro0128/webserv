#include "Document.hpp"

Document::Document()
{

}
Document::~Document()
{

}
Document::Document(const Document& r)
{
    _incomplete = r._incomplete;
    _complete = r._complete;
    _excute = r._excute;
}
Document& Document::operator=(const Document& r)
{
    if (this != &r)
    {
        _incomplete = r._incomplete;
        _complete = r._complete;
        _excute = r._excute;
    }
    return *this;
}
void Document::putIncomplete(int fd, Request& req)
{
    _incomplete[fd] = req;
}
void Document::removeIncomplete(int fd)
{
    if (_incomplete.find(fd) != _incomplete.end())
        _incomplete.erase(fd);
}
Request& Document::getIncomplete(int fd)
{
    return _incomplete[fd];
}
void Document::putComplete(Request& req)
{
    _complete.push_back(req);
}
void Document::removeComplete(int fd)
{
    if (_complete.empty())
        throw std::runtime_error("empty complete list");
    for (std::vector<Request>::iterator it = _complete.begin(); it != _complete.end(); it++)
    {
        if (it->getFd() == fd)
        {
            _complete.erase(it);
            break ;
        }
    }
}
std::vector<Request>& Document::getComplete()
{
	return _complete;
}

void Document::putExcute(int pid, Request& req)
{
    if (_excute.find(pid) != _excute.end())
        _excute[pid] = req;
    else
        _excute.insert(std::pair<int, Request>(pid, req));
}
void Document::removeExcute(int pid)
{
    if (_excute.find(pid) != _excute.end())
        _excute.erase(pid);
}
Request& Document::getExcute(int pid)
{
    if (_excute.find(pid) != _excute.end())
        return _excute[pid];
    else
        throw std::runtime_error("no such pid");
}