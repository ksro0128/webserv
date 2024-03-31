#ifndef DOCUMENT_HPP
# define DOCUMENT_HPP

# include "include.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ExecInfo.hpp"

class Document{
public:
    Document();
    ~Document();
    Document(const Document& r);
    Document& operator=(const Document& r);

    void PutIncomplete(int fd, Request& req);
    void RemoveIncomplete(int fd);
    Request& GetIncomplete(int fd); // 반환받을 일이 있다면 참조로반환받고 밖에서 삭제해주는것이 맞나?

    void PutComplete(Request& req);
    void RemoveComplete();
    std::vector<Request>& GetComplete(); // 이것도 ㅇㅇ

    void PutExcute(int pipefd);
    void RemoveExcute(int pipefd);
    std::string& GetBuffer(int pipefd); // 이것도 ㅇㅇ
    std::map<int, std::string>& GetExcute();


    void PutPidInfo(int pipefd, ExecInfo& info);
    void RemovePidInfo(int pipefd);
    ExecInfo& GetPidInfo(int pid);

    int GetExcuteCount();
    void AddExcuteCount();
    void SubExcuteCount();
	
    

    void PutResponse(Response& res);
    std::vector<Response>& GetResponse();

	void PutStatic(Request& req);
	std::vector<Request>& GetStatic();
	void ClearStatic();

	void PutDynamic(Request& req);
	std::vector<Request>& GetDynamic();
	void ClearDynamic();


    void PutFdEvent(int fd, const std::string& identifier);
    std::multimap<int, std::string>& GetFdEvent();

private:
    int m_excuteCount;
    std::map<int, Request> m_incomplete;
    std::vector<Request> m_complete; // 실행부로 넘기는거
    std::map<int, std::string> m_excute; // 실행부에서 등록,삭제,확인 pipefd, buffer
    std::map<int, ExecInfo> m_pidInfo;
    std::vector<Response> m_response;
	std::vector<Request> m_static;
	std::vector<Request> m_dynamic;

    std::multimap<int, std::string> m_fd_identifier;
};

#endif