#ifndef DOCUMENT_HPP
# define DOCUMENT_HPP

# include "include.hpp"
# include "Request.hpp"

class Document{
public:
    Document();
    ~Document();
    Document(const Document& r);
    Document& operator=(const Document& r);
    void putIncomplete(int fd, Request& req);
    void removeIncomplete(int fd);
    Request& getIncomplete(int fd); // 반환받을 일이 있다면 참조로반환받고 밖에서 삭제해주는것이 맞나?
    void putComplete(Request& req);
    void removeComplete(int fd);
    Request& getComplete(int fd); // 이것도 ㅇㅇ
    void putExcute(int pid, Request& req);
    void removeExcute(int pid);
    Request& getExcute(int pid); // 이것도 ㅇㅇ
    // void putResponse(Response& res);
    // void removeResponse(int pid);
    // Response& getResponse(int pid);
private:
    std::map<int, Request> _incomplete;
    std::vector<Request> _complete; // 실행부로 넘기는거
    std::map<int, Request> _excute; // 실행부에서 등록,삭제,확인
    // std::vector<Response> _response;
};

#endif