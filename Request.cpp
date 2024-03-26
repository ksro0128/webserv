#include "Request.hpp"
#include "include.hpp"


Request::Request() : _status(0) 
{
    _buff.reserve(1024);
    _remain = "";
    _status = 200;
    _req_close = 0;
    _eof = 0;
    _body_limit = 100000;
    _read_body_len = 0;
    _req_body_len = 0;
    _stage = 0;
    _chunked = 0;
    _complete = 0;
    _startline_cnt = 0;
    _end = 0;
}

Request::~Request() {}

Request::Request(const Request& rhs)
{
    *this = rhs;
}



Request& Request::operator=(const Request& r)
{
    if (this == &r)
        return *this;
    _buff = r._buff;
    _method = r._method;
    _path = r._path;
    _version = r._version;
    _headers = r._headers;
    _body = r._body;
    _remain = r._remain;
    _host = r._host;
    _status = r._status;
    _origin_fd = r._origin_fd;
    _host_port = r._host_port;
    _startline_cnt = r._startline_cnt;
    _req_close = r._req_close;
    _read_body_len = r._read_body_len;
    _req_body_len = r._req_body_len;
    _body_limit = r._body_limit;
    _content_length = r._content_length;
    _chunked = r._chunked;
    _eof = r._eof;
    _stage = r._stage;
    _complete = r._complete;
    _end = r._end;
    return *this;
}

void Request::parseStartline(const std::string& s) 
{
    std::string           tmp;
    unsigned long         pos;
    int                   len1 = 0, len2 = 0;

    if (_startline_cnt == 0 && (pos = s.find(" ")) != std::string::npos)
    {
        _method = s.substr(0, pos);
        len1 = _method.length();
        if (!checkMethod(_method) && _status == 0)
            _status = 501;
        _startline_cnt++;
    }
    if (_startline_cnt == 1 && (pos = s.find(" ", len1 + 1)) != std::string::npos)
    {
        _path = s.substr(len1 + 1, pos - len1 - 1);
        len2 = _path.length();
        _startline_cnt++;
    }
    if (_startline_cnt == 2 && (pos = s.find("\r\n", len1 + len2 + 2)) != std::string::npos)
    {
        _version = s.substr(len1 + len2 + 2, pos - len1 - len2 - 2);
        if (!checkVersion(_version) && _status == 0)
            _status = 505;
        _startline_cnt++;
    }
    if (_startline_cnt == 3)
        _stage = 1;
}

void Request::parseHeader(const std::string& s) 
{
    std::string         key, value;
    int                 key_len;
    unsigned long       pos;

    if (s == "\r\n")
    {
        _stage = 2;
        checkEseential();
        return ;
    }
    if ((pos = s.find(":")) != std::string::npos)
    {
        key = s.substr(0, pos);
        key_len = key.length();
        for (int i = 0; i < key_len; i++)
            key[i] = std::tolower(key[i]);
        if (!check_key(key) && _status == 0)
        {
            std::cout << "key check error\n";
            _status = 400;
            _req_close = 1;
        }
        value = s.substr(pos + 1, s.length() - key_len - 3);
        if (value == "")
        {
            std::cout << "value is empty\n";
            _status = 400;
            _req_close = 1;
        }
        remove_ows(value);
        _headers.insert(std::pair<std::string, std::string>(key, value));
    }
    else
    {
        std::cout << "header parsing error\n";
        _status = 400;
        _req_close = 1;
    }
}

void Request::parseRequest(int fd, std::string& buff) 
{
    std::string tmp;
    int  start,end;
    int  tmp_len;
    int  total_len;

    static int i = 0;
    i++;
    _origin_fd = fd;
    start = 0;
    end = 0;
    tmp = buff;
    _buff = tmp;
    if (_remain.length() > 0)
    {
        _buff = _remain + _buff;
        _remain = "";
    }
    total_len = _buff.length();
    while (_stage < 2 && (tmp = getLine(start, _buff)) != "")
    {
        tmp_len = tmp.length();
        start += tmp_len;
        if (tmp.find("\r\n") == std::string::npos || _end == 1)
        {
            _remain += tmp;
            // std::cout << "\n\n\nremain!\n";
            // std::cout << _remain << std::endl;
            // std::cout << "\n\n";
            // std::cout << "_buff is " << _buff << "\n\n";
            break;
        }
        switch (_stage)
        {
            case 0:
                parseStartline(tmp);
                break;
            case 1:
                parseHeader(tmp);
                break;
            default:
                break;
        }
    }
    if (_stage == 2 && _end == 0)
    {
        if (_req_body_len <= total_len - start)
        {
            _body += _buff.substr(start, _req_body_len);
            _remain = _buff.substr(start + _req_body_len, total_len - start - _req_body_len);
            _read_body_len += _req_body_len;
            _req_body_len = 0;
            _end = 1;
            _complete = 1;
        }
        else
        {
            _body += _buff.substr(start, total_len - start);
            _read_body_len += total_len - start;
            _req_body_len -= total_len - start;
        }
    }
    if (end == 1)
        _remain += _buff.substr(start, total_len - start);
}

void Request::printRequest() 
{
    std::cout << "Request " << "error:" << _status << "{" << std::endl;
    std::cout << "Method : <" << _method << ">" << std::endl;
    std::cout << "Path : <" << _path << ">" << std::endl;
    std::cout << "Version : <" << _version << ">" << std::endl;
    std::cout << "Headers{" << std::endl;
    for (std::multimap<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
    std::cout << "}" << std::endl;
    std::cout << "Body : " << _body << std::endl;
    std::cout << "}" << std::endl;
}

int Request::checkMethod(const std::string& s) 
{
    if (s == "GET" || s == "POST" || s == "DELETE" || s == "HEAD")
        return (1);
    return (0);
}

int Request::checkVersion(const std::string& s) 
{
    if (s == "HTTP/1.1")
        return (1);
    return (0);
}

int Request::check_key(const std::string& s) 
{
    char    arr[] = "()<>@,;:\\\"/[]?={} \t\r\n\"\\^`|~";
    int     s_len = s.length();
    for (int i = 0; i < s_len; i++)
    {
        for (int j = 0; j < arr[j]; j++)
        {
            if (s[i] == arr[j])
                return (0);
        }
    }
    return (1);
}

void Request::remove_ows(std::string& s) 
{
    int i = 0;
    int j = s.length() - 1;
    if (s.length() == 0)
        return ;
    while (s[i] && s[i] == ' ')
        i++;
    while (s[j] && s[j] == ' ')
        j--;
    if (j == 0)
        s = "";
    else
        s = s.substr(i, j - i + 1);
}

void Request::checkEseential() 
{
    std::string tmp, hostport;
    if ((_method == "GET" || _method == "HEAD"))
    {
        if (_headers.find("content-length") != _headers.end())
        {
            _status = 400;
            std::cout << "GET or HEAD method should not have content-length\n";
            _req_body_len = 0;
            _req_close = 1;
        }
        else
        {
            _req_body_len = 0;
            _complete = 1;
        }
        _end = 1;
    }
    else if (_headers.find("host") == _headers.end() || _headers.find("host")->second == "" || _headers.count("host") > 1)
    {
        _status = 400;
        std::cout << "Host header is missing or invalid\n";
        _req_close = 1;
        _end = 1;
    }
    else
    {
        if (_headers.find("content-length") == _headers.end())
        {
            _status = 400;
            std::cout << "Content-Length header is missing\n";
            _req_close = 1;
            _end = 1;
        }
        else
        {
            tmp = _headers.find("content-length")->second;
            if (isNumber(_headers.find("content-length")->second) == 0)
            {
                _status = 400;
                std::cout << "Content-Length header is not a number\n";
                _req_close = 1;
                _end = 1;
            }
            else
            {
                _content_length = atoi(tmp.c_str());
                _req_body_len = _content_length;
                if (_content_length < 0 || _content_length > _body_limit || _headers.count("content-length") > 1)
                {
                    _status = 400;
                    std::cout << "Content-Length header is invalid\n";
                    _req_close = 1;
                    _end = 1;
                }
            }
        }
        unsigned long pos;
        tmp = _headers.find("host")->second;
        if (pos = tmp.find(":"), pos != std::string::npos)
        {
            _host = tmp.substr(0, pos);
            hostport = tmp.substr(pos + 1, tmp.length() - pos - 1);
            if (isNumber(hostport) == 0)
            {
                _status = 400;
                std::cout << "Port number is not a number\n";
                _req_close = 1;
                _end = 1;
            }
            else
            {
                _host_port = atoi(hostport.c_str());
                if (_host_port < 0 || _host_port > 65535 || errno == ERANGE)
                {
                    _status = 400;
                    std::cout << "Port number is invalid\n";
                    _req_close = 1;
                    _end = 1;
                }
            }
        }
        else
        {
            _host = tmp;
            _host_port = 80;
        }
    }
}

int Request::isNumber(const std::string& s) 
{
    long long num = 0;
    for (unsigned long i = 0; i < s.length(); i++)
    {
        if (!isdigit(s[i]))
            return (0);
        num = num * 10 + (s[i] - '0');
        if (num < 0)
            return (0);
    }
    return (1);
}

std::string Request::getLine(int start, std::string& s) 
{
    unsigned long pos;
    std::string tmp;

    if ((pos = s.find("\r\n", start)) != std::string::npos)
    {
        tmp = s.substr(start, pos - start + 2);
        return (tmp);
    }
    else
        return (s.substr(start, s.length() - start));
}

int Request::getFd() 
{
    return (_origin_fd);
}

int Request::isComplete() 
{
    return (_complete);
}

std::multimap<std::string, std::string>& Request::getHeaders() 
{
    return (_headers);
}

std::string& Request::getSpecificHeader(const std::string& key) 
{
    if (_headers.find(key) == _headers.end())
        throw std::runtime_error("Header not found");
    return (_headers.find(key)->second);
}

std::string& Request::getMethod() 
{
    return (_method);
}

std::string& Request::getPath() 
{
    return (_path);
}

std::string& Request::getVersion() 
{
    return (_version);
}

std::string& Request::getBody() 
{
    return (_body);
}

std::string& Request::getHost() 
{
    return (_host);
}

int Request::getPort() 
{
    return (_host_port);
}

int Request::getStatus() 
{
    return (_status);
}