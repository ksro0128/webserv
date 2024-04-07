#ifndef EXECINFO_HPP
# define EXECINFO_HPP

# include "include.hpp"
# include "Request.hpp"

class ExecInfo{
private:
    int         m_socket;
    int         m_readPipe;
    int         m_writePipe;
    int         m_pid;
    Request m_req;
    std::string m_buffer;
public:
    ExecInfo();
    ExecInfo(int socket, int readPipe, int writePipe, int pid, Request& req);
    ~ExecInfo();
    ExecInfo(const ExecInfo& r);
    ExecInfo& operator=(const ExecInfo& r);

    int GetSocket();
    int GetReadPipe();
    int GetWritePipe();
    int GetPid();
    std::string& GetBuffer();
    Request& GetRequest();
};

#endif