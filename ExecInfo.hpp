#ifndef EXECINFO_HPP
# define EXECINFO_HPP

# include "include.hpp"
# include "Request.hpp"

class ExecInfo{
private:
    int m_socket;
    int m_readPipe;
    int m_pid;
    Request m_req;
public:
    ExecInfo();
    ExecInfo(int socket, int readPipe, int pid, Request& req);
    ~ExecInfo();
    ExecInfo(const ExecInfo& r);
    ExecInfo& operator=(const ExecInfo& r);


    int GetSocket();
    int GetReadPipe();
    int GetPid();
    Request& GetRequest();
};

#endif