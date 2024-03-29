#ifndef CGIPROCESSOR_HPP
# define CGIPROCESSOR_HPP

# include "include.hpp"
# include "Request.hpp"
# include "Document.hpp"
# include "Server.hpp"
# include "Config.hpp"

class CgiProcessor
{
public:
    CgiProcessor();
    CgiProcessor(const CgiProcessor &other);
    void Set(Config &config, int kq);
    CgiProcessor &operator=(const CgiProcessor &other);
    ~CgiProcessor();
    void ExcuteCgi(Document &doc);
    void MakeResponse(Document &doc, int readFd);
private:
    Config m_config;
    int m_kq;
    std::map<int, std::string> m_statusMessageSet;
    std::vector<std::string> getCgiInfo(Request &request, Server &server);
    void setResponseError(Request &request, Response &response, Server &server, int status);
};

#endif