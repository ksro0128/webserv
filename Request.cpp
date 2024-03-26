// #include "Request.hpp"
// #include "include.hpp"


// Request::Request() : _status(0) 
// {
//     _buff.reserve(1024);
//     _remain = "";
//     _req_close = 0;
//     _eof = 0;
//     _body_limit = 100000;
//     _body_len = 0;
//     _stage = 0;
//     _chunked = 0;
//     _startline_cnt = 0;
// }

// Request::~Request() {}

// Request::Request(const Request& rhs) : _status(0), _buff(rhs._buff), _method(rhs._method), _path(rhs._path), _version(rhs._version), _headers(rhs._headers), _body(rhs._body)
// {
//     _buff = rhs._buff;
//     _method = rhs._method;
//     _path = rhs._path;
//     _version = rhs._version;
//     _headers = rhs._headers;
//     _body = rhs._body;
//     _status = rhs._status;
// }

// Request& Request::operator=(const Request& r) {

//     if (this == &r)
//         return *this;
//     _buff = r._buff;
//     _method = r._method;
//     _path = r._path;
//     _version = r._version;
//     _headers = r._headers;
//     _body = r._body;
//     _status = r._status;
//     return *this;
// }

// int Request::parseStartline(const std::string& s) 
// {
//     std::string tmp;
//     int         pos;
//     int         ret = 0;
//     int         len1 = 0, len2 = 0;

//     if (_startline_cnt == 0 && (pos = s.find(" ")) != std::string::npos)
//     {
//         _method = s.substr(0, pos);
//         len1 = _method.length();
//         if (!checkMethod(_method) && _status == 0)
//             _status = 501;
//         _startline_cnt++;
//     }
//     if (_startline_cnt == 1 && (pos = s.find(" ", len1 + 1)) != std::string::npos)
//     {
//         _path = s.substr(len1 + 1, pos - len1 - 1);
//         len2 = _path.length();
//         _startline_cnt++;
//     }
//     if (_startline_cnt == 2 && (pos = s.find("\r\n", len1 + len2 + 2)) != std::string::npos)
//     {
//         _version = s.substr(len1 + len2 + 2, pos - len1 - len2 - 2);
//         if (!checkVersion(_version) && _status == 0)
//             _status = 505;
//         else if (s[pos + 1] != '\0')
//             _status = 400;
//         _startline_cnt++;
//     }
//     if (_startline_cnt == 3)
//         _stage = 1;
// }

// int Request::parseHeader(const std::string& s) 
// {
//     std::string key, value;
//     int         key_len;
//     int         pos;
//     int         end;

//     if (s == "\r\n")
//     {
//         _stage = 2;
//         checkEseential();
//         return (0);
//     }
//     if ((pos = s.find(":")) != std::string::npos)
//     {
//         std::cout << "original : " << s << std::endl;
//         key = s.substr(0, pos);
//         key_len = key.length();
//         for (int i = 0; i < key_len; i++)
//             key[i] = std::tolower(key[i]);
//         if (!check_key(key) && _status == 0)
//             _status = 400;
//         end = s.find("\r\n");
//         std::cout << pos << " , " << end << std::endl;
//         value = s.substr(pos, s.length() - key_len - 2);
//         remove_ows(value);
//         _headers.insert(std::pair<std::string, std::string>(key, value));
//     }
// }

// void Request::parseRequest(int fd) 
// {
//     std::string tmp;
//     char buff[1024];
//     int  bytes;
//     int  start,end;
//     int  tmp_len;

//     start = 0;
//     end = 0;
//     _status = 0;
//     bytes = read(fd, buff, 1024);
//     if (bytes == 0)
//     {
//         _eof = 1;
//         std::cout << "Client " << fd << " closed connection" << std::endl;
//         return ;
//     }
//     else if (bytes < 0)
//         throw std::runtime_error("Error reading from socket");
//     else
//     {
//         _buff = buff;
//         if (_remain.length() > 0)
//         {
//             _buff = _remain + _buff;
//             _remain = "";
//         }
//         tmp = getLine(start, _buff);
//         tmp_len = tmp.length();
//         start += tmp_len;
//         while (tmp != "")
//         {
//             if (tmp.find("\r\n") == std::string::npos)
//             {
//                 _remain = tmp;
//                 break;
//             }
//             switch (_stage)
//             {
//                 case 0:
//                     parseStartline(tmp);
//                     break;
//                 case 1:
//                     parseHeader(tmp);
//                     break;
//                 case 2:
//                 {
//                     if (_body_len < _body_limit && _body_len + tmp_len < _body_limit)
//                     {
//                         _body += tmp;
//                         _body_len += tmp_len;
//                     }
//                     else
//                     {
//                         _status = 413;
//                         _req_close = 1;
//                     }
//                 }
//                 default:
//                     break;
//             }
//             tmp = getLine(start, _buff);
//             tmp_len = tmp.length();
//             start += tmp_len + 2;
//         }
//     }
// }

// void Request::printRequest() 
// {
//     std::cout << "Request " << "error:" << _status << "{" << std::endl;
//     std::cout << "Method : <" << _method << ">" << std::endl;
//     std::cout << "Path : <" << _path << ">" << std::endl;
//     std::cout << "Version : <" << _version << ">" << std::endl;
//     std::cout << "Headers{" << std::endl;
//     for (std::multimap<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
//     {
//         std::cout << it->first << " : " << it->second << std::endl;
//     }
//     std::cout << "}" << std::endl;
//     std::cout << "Body : " << _body << std::endl;
//     std::cout << "}" << std::endl;
// }

// int Request::checkMethod(const std::string& s) 
// {
//     if (s == "GET" || s == "POST" || s == "DELETE" || s == "HEAD")
//         return (1);
//     return (0);
// }

// int Request::checkVersion(const std::string& s) 
// {
//     if (s == "HTTP/1.1")
//         return (1);
//     return (0);
// }

// int Request::check_key(const std::string& s) 
// {
//     char    arr[] = "()<>@,;:\\\"/[]?={} \t\r\n\"\\^`|~";
//     int     s_len = s.length();
//     for (int i = 0; i < s_len; i++)
//     {
//         for (int j = 0; j < arr[j]; j++)
//         {
//             if (s[i] == arr[j])
//                 return (0);
//         }
//     }
//     return (1);
// }

// void Request::remove_ows(std::string& s) 
// {
//     int i = 0;
//     int j = s.length() - 1;
//     if (s.length() == 0)
//         return ;
//     while (s[i] && s[i] == ' ')
//         i++;
//     while (s[j] && s[j] == ' ')
//         j--;
//     if (j == 0)
//         s = "";
//     else
//         s = s.substr(i, j - i + 1);
// }

// int Request::checkEseential() 
// {
//     if ((_method == "GET" || _method == "HEAD") && _headers.find("content-length") != _headers.end())
//     {
//         _status = 400;
//         _req_close = 1;
//     }
//     else if (_headers.find("HOST") == _headers.end())
//     {
//         _status = 400;
//         _req_close = 1;
//     }
//     else
//     {
//         if (_headers.find("content-length") == _headers.end())
//         {
//             _status = 400;
//             _req_close = 1;
//         }
//         else
//         {
//             if (isNumber(_headers.find("content-length")->second) == 0)
//             {
//                 _status = 400;
//                 _req_close = 1;
//             }
//             else
//             {
//                 _body_len = atoi(_headers.find("content-length")->second.c_str());
//                 if (_body_len < 0 || _body_len > 100000)
//                 {
//                     _status = 400;
//                     _req_close = 1;
//                 }
//             }
//         }
//     }
// }

// int Request::isNumber(const std::string& s) 
// {
//     long long num = 0;
//     for (int i = 0; i < s.length(); i++)
//     {
//         if (!isdigit(s[i]))
//             return (0);
//         num = num * 10 + (s[i] - '0');
//         if (num < 0)
//             return (0);
//     }
//     return (1);
// }

// std::string Request::getLine(int start, const std::string& s) 
// {
//     int pos;
//     std::string tmp;

//     if ((pos = s.find("\r\n", start)) != std::string::npos)
//     {
//         tmp = s.substr(start, pos - start + 2);
//         return (tmp);
//     }
//     return ("");
// }