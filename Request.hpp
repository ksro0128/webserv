// #ifndef REQUEST_HPP
// # define REQUEST_HPP

// # include "include.hpp"

 

// class Request{
// public:
//     Request();
//     ~Request();
//     Request(const Request& r);
//     Request& operator=(const Request& r);
//     void parseRequest(int fd);
//     std::string&    getMethod();
//     std::string&    getPath();
//     std::string&    getVersion();
//     std::map<std::string, std::string>& getHeaders();
//     std::string&    getBody();
//     bool            isComplete();
//     void            printRequest();
// private:
//     std::string     _buff;
//     std::string     _method;
//     std::string     _path;
//     std::string     _version;   
//     std::multimap<std::string, std::string> _headers;
//     std::string     _body;
//     std::string     _remain;
//     int             _status;
//     int             _startline_cnt;
//     int             _req_close;
//     int             _body_len;
//     int             _body_limit;
//     int             _content_length;
//     int             _chunked;
//     int             _eof;
//     int             _stage;
//     int             parseStartline(const std::string& s);
//     int             checkMethod(const std::string& s);
//     int             checkVersion(const std::string& s);
//     int             parseHeader(const std::string& s);
//     int             check_key(const std::string& s);
//     int             checkEseential();
//     void            remove_ows(std::string& s);
//     int             isNumber(const std::string& s);
//     std::string     getLine(int start, const std::string& s);
// };

// #endif