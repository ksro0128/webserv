#ifndef DOCUMENT_HPP
# define DOCUMENT_HPP

# include "include.hpp"
# include "Request.hpp"
# include "Response.hpp"

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

    void PutExcute(int pid, Request& req);
    void RemoveExcute(int pid);
    Request& GetExcute(int pid); // 이것도 ㅇㅇ
	
    void PutResponse(Response& res);
    std::vector<Response>& GetResponse();
private:
    std::map<int, Request> m_incomplete;
    std::vector<Request> m_complete; // 실행부로 넘기는거
    std::map<int, Request> m_excute; // 실행부에서 등록,삭제,확인
    std::vector<Response> m_response;
};

#endif