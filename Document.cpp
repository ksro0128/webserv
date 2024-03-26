#include "Document.hpp"

Document::Document()
{

}
Document::~Document()
{

}
Document::Document(const Document& r)
{

}
Document& Document::operator=(const Document& r)
{

}
void Document::putIncomplete(int fd, Request& req)
{
    _incomplete[fd] = req;
}
void Document::removeIncomplete(int fd)
{
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
    for (std::vector<Request>::iterator it = _complete.begin(); it != _complete.end(); it++)
    {
        if (it->getFd() == fd)
        {
            _complete.erase(it);
            break ;
        }
    }
}
Request& Document::getComplete(int fd)
{
    if (_complete.empty())
        throw std::exception();
    for (std::vector<Request>::iterator it = _complete.begin(); it != _complete.end(); it++)
    {
        if (it->getFd() == fd)
            return *it;
    }
}
void Document::putExcute(int pid, Request& req)
{
    _
}
void Document::removeExcute(int pid)
{

}
Request& Document::getExcute(int pid)
{

}