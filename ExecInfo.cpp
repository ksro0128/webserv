#include "ExecInfo.hpp"

ExecInfo::ExecInfo()
{
}

ExecInfo::ExecInfo(int socket, int readPipe, int writePipe, int pid,  Request& req)
    : m_socket(socket), m_readPipe(readPipe), m_writePipe(writePipe), m_pid(pid), m_req(req)
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
        m_writePipe = r.m_writePipe;
        m_req = r.m_req;
        m_pid = r.m_pid;
        m_buffer = r.m_buffer;
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

int ExecInfo::GetWritePipe()
{
    return m_writePipe;
}

Request& ExecInfo::GetRequest()
{
    return m_req;
}


int ExecInfo::GetPid()
{
    return m_pid;
}

std::string& ExecInfo::GetBuffer()
{
    return m_buffer;
}