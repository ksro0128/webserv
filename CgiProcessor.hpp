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
    void Set(Config &config, int kq, std::vector<std::string> envp);
    CgiProcessor &operator=(const CgiProcessor &other);
    ~CgiProcessor();
    void ExcuteCgi(Document &doc);
    void Read(Document &doc, int fd);
    void Write(Document &doc, int fd);
    void Wait(Document &doc, int pid);
private:
    Config m_config;
    int m_kq;
    std::map<int, std::string> m_statusMessageSet;
	std::vector<std::string> m_envp;
    std::vector<std::string> getCgiInfo(Request &request, Server &server);
    void setResponseError(Request &request, Response &response, Server &server, int status);
    void setWriteEvent(int fd);
    void setReadEvent(int fd);
    void setPidEvent(int pid);
    void inChild(Request &request, Server &server, std::vector<std::string> &cgi, std::string& filename, int p1[2], int p2[2]);
	bool isAllowedMethod(Request& request, Location& location);
	void addCgiEnv(std::string key, std::string value);
	std::string inttoString(int num);
};

#endif