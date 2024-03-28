#ifndef EXECINFO_HPP
# define EXECINFO_HPP

# include "include.hpp"
# include "Request.hpp"

class ExecInfo{
private:
    int m_socket;
    int m_readPipe;
    Request m_req;
public:
    ExecInfo();
    ExecInfo(int socket, int readPipe, Request& req);
    ~ExecInfo();
    ExecInfo(const ExecInfo& r);
    ExecInfo& operator=(const ExecInfo& r);


    int GetSocket();
    int GetReadPipe();
    Request& GetRequest();
};

#endif