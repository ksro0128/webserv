#ifndef CGIPROCESSOR_HPP
# define CGIPROCESSOR_HPP

# include "include.hpp"
# include "Request.hpp"
# include "Document.hpp"
# include "Server.hpp"
# include "Config.hpp"
# include "Response.hpp"

class CgiProcessor
{
public:
    CgiProcessor();
    CgiProcessor(const CgiProcessor &other);
    void Set(Config &config, int kq);
    CgiProcessor &operator=(const CgiProcessor &other);
    ~CgiProcessor();
    void ExcuteCgi(Document &doc);
    void Read(Document &doc, int fd);
    void Wait(Document &doc, int pid);
private:
    Config m_config;
    int m_kq;
    std::map<int, std::string> m_statusMessageSet;
    std::vector<std::string> getCgiInfo(Request &request, Server &server);
    void setResponseError(Request &request, Response &response, Server &server, int status);
    void setWriteEvent(int fd);
    void setReadEvent(int fd);
    void setPidEvent(int pid);
    void inChild(Request &request, Server &server, std::vector<std::string> &cgi, std::string& filename, int p[2]);
};

#endif