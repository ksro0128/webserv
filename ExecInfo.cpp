#include "ExecInfo.hpp"

ExecInfo::ExecInfo()
{
}

ExecInfo::ExecInfo(int socket, int readPipe, Request& req)
    : m_socket(socket), m_readPipe(readPipe), m_req(req)
{
}

ExecInfo::~ExecInfo()
{
}

ExecInfo::ExecInfo(const ExecInfo& r)
{
    *this = r;
}

ExecInfo& ExecInfo::operator=(const ExecInfo& r)
{
    if (this != &r)
    {
        m_socket = r.m_socket;
        m_readPipe = r.m_readPipe;
        m_req = r.m_req;
    }
    return *this;
}

int ExecInfo::GetSocket()
{
    return m_socket;
}   

int ExecInfo::GetReadPipe()
{
    return m_readPipe;
}

Request& ExecInfo::GetRequest()
{
    return m_req;
}