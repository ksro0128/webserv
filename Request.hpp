#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "include.hpp"

 


class Request{
public:
    Request();
    ~Request();
    Request(const Request& r);
    Request& operator=(const Request& r);
    void parseRequest(int fd, std::string& buff);
    std::string&    getMethod();
    std::string&    getPath();
    std::string&    getVersion();
    std::multimap<std::string, std::string>& getHeaders();
    std::string&    getBody();
    int             isComplete();
    void            printRequest();
    int             getFd();
    std::string&    getHost();
    int             getPort();
    int             getStatus();
    int             setStatus(int status);
    std::string&    getSpecificHeader(const std::string& key);
private: //essential
    std::string     _buff;
    std::string     _method;
    std::string     _path;
    std::string     _version;   
    std::multimap<std::string, std::string> _headers;
    std::string     _body;
    std::string     _remain;
    std::string     _host;
    int             _status;
    int             _origin_fd;
    int             _host_port;
    int             _startline_cnt;
    int             _req_close;
    int             _read_body_len;
    int             _req_body_len;
    int             _body_limit;
    int             _content_length;
    int             _chunked;
    int             _eof;
    int             _stage;
    int             _complete; // 완성했는가?
    int             _end; // 더 저장할 필요가 있는가?
    void            parseStartline(const std::string& s);
    int             checkMethod(const std::string& s);
    int             checkVersion(const std::string& s);
    void            parseHeader(const std::string& s);
    int             check_key(const std::string& s);
    void            checkEseential();
    void            remove_ows(std::string& s);
    int             isNumber(const std::string& s);
    std::string     getLine(int start, std::string& s);
};

#endif

// 1. 1024바이트 안에 요청이 두개 들어있는 경우
// -> 헤더까지 읽든 바디까지 다 읽든 간에, 헤더끝나고 바디없거나 바디다읽은 경우 나머지를
// remain에다가 저장한 뒤에 _complete로 표시한다
// 이 경우, 처리하는 클래스쪽에서 remain을 제외하고 전부 초기화해줘야 한다

// 2. 1024바이트 안에 헤더가 끝나지 않은 경우
// -> 이건 이미 구현했음. 그러나 다음 읽기에서 헤더도끝나고 바디도끝났는데 다음요청있는거 생각해야함

// 3. 1024바이트 안에 헤더가 끝나고 바디가 있는데 덜끝난 경우
// -> 이것도 구현은 했는데 마찬가지로 1번이 완벽하게 처리되어야 한다.

// 그러면 바디를 읽어야할 경우는 헤더파싱끝났는데 content-length가 있는경우뿐임.(일단은)



// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
// 012301230123012301230123


// host없으면?