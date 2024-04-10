#include "Request.hpp"
#include "include.hpp"


Request::Request() : m_status(0) 
{
    m_buff.reserve(1024);
    m_remain = "";
    m_query = "";
    m_status = 200;
    m_reqClose = 0;
    m_eof = 0;
    m_bodyLimit = 100000000;
    m_readBodyLen = 0;
    m_reqBodyLen = 0;
    m_stage = 0;
    m_chunked = 0;
    m_complete = 0;
    m_startline_cnt = 0;
    m_end = 0;
    m_chunkedFlag = 0;
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
    m_buff = r.m_buff;
    m_method = r.m_method;
    m_path = r.m_path;
    m_version = r.m_version;
    m_headers = r.m_headers;
    m_body = r.m_body;
    m_remain = r.m_remain;
    m_host = r.m_host;
    m_status = r.m_status;
    m_origin_fd = r.m_origin_fd;
    m_host_port = r.m_host_port;
    m_startline_cnt = r.m_startline_cnt;
    m_reqClose = r.m_reqClose;
    m_readBodyLen = r.m_readBodyLen;
    m_reqBodyLen = r.m_reqBodyLen;
    m_bodyLimit = r.m_bodyLimit;
    m_contentLength = r.m_contentLength;
    m_chunked = r.m_chunked;
    m_eof = r.m_eof;
    m_stage = r.m_stage;
    m_complete = r.m_complete;
    m_end = r.m_end;
    m_query = r.m_query;
    m_chunkedFlag = r.m_chunkedFlag;
    return *this;
}

void Request::parseStartline(const std::string& s) 
{
    std::string           tmp;
    unsigned long         pos;
    int                   len1 = 0, len2 = 0;

    if (m_startline_cnt == 0 && (pos = s.find(" ")) != std::string::npos)
    {
        m_method = s.substr(0, pos);
        len1 = m_method.length();
        if (!checkMethod(m_method) && m_status == 0)
            m_status = 501;
        m_startline_cnt++;
    }
    if (m_startline_cnt == 1 && (pos = s.find(" ", len1 + 1)) != std::string::npos)
    {
        m_path = s.substr(len1 + 1, pos - len1 - 1);
        len2 = m_path.length();
        unsigned long pos2;
        if ((pos2 = m_path.find("?")) != std::string::npos)
        {
            m_query = m_path.substr(pos2 + 1, m_path.length() - pos2 - 1);
            m_path = m_path.substr(0, pos2);
        }
        m_startline_cnt++;
    }
    if (m_startline_cnt == 2 && (pos = s.find("\r\n", len1 + len2 + 2)) != std::string::npos)
    {
        m_version = s.substr(len1 + len2 + 2, pos - len1 - len2 - 2);
        if (!checkVersion(m_version) && m_status == 0)
            m_status = 505;
        m_startline_cnt++;
    }
    if (m_startline_cnt == 3)
        m_stage = 1;
}

void Request::parseHeader(const std::string& s) 
{
    std::string         key, value;
    int                 key_len;
    unsigned long       pos;

    if (s == "\r\n")
    {
        m_stage = 2;
        checkEssential();
        return ;
    }
    if ((pos = s.find(":")) != std::string::npos)
    {
        key = s.substr(0, pos);
        key_len = key.length();
        for (int i = 0; i < key_len; i++)
            key[i] = std::tolower(key[i]);
        if (!check_key(key) && m_status == 0)
        {
            std::cout << "key check error\n";
            m_status = 400;
            m_reqClose = 1;
        }
        value = s.substr(pos + 1, s.length() - key_len - 3);
        if (value == "")
        {
            std::cout << "value is empty\n";
            m_status = 400;
            m_reqClose = 1;
        }
        remove_ows(value);
        m_headers.insert(std::pair<std::string, std::string>(key, value));
    }
    else
    {
        std::cout << "header parsing error\n";
        std::cout << "error header is " << s << "\n";
        m_status = 400;
        m_reqClose = 1;
    }
}

void Request::ParseRequest(int fd, std::string& buff) 
{
    std::string tmp;
    int  start;
    int  tmp_len;
    int  total_len;
    int  len;

    m_origin_fd = fd;
    start = 0;
    tmp = buff;
    // std::cout << "In ParseRequest\n";
	// std::cout << tmp << std::endl;
    // std::cout << "End ParseRequest\n";
    m_buff = tmp;
    if (m_remain.length() > 0)
    {
        m_buff = m_remain + m_buff;
        m_remain = "";
    }
    total_len = m_buff.length();
    while (m_stage < 2 && (tmp = getLine(start, m_buff)) != "")
    {
        tmp_len = tmp.length();
        start += tmp_len;
        if (tmp.find("\r\n") == std::string::npos || m_end == 1)
        {
            m_remain += tmp;
            // std::cout << "\n\n\nremain!\n";
            // std::cout << m_remain << std::endl;
            // std::cout << "\n\n";
            // std::cout << "m_buff is " << m_buff << "\n\n";
            break;
        }
        switch (m_stage)
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
    if (m_stage == 2 && m_end == 0)
    {
        if (m_chunked == 1)
        {
            while (m_complete == 0)
            {
                // std::cout << "in chunked part\n";
                tmp = getLine(start, m_buff);
                if (tmp.find("\r\n") == std::string::npos)
                {
                    m_remain = tmp;
                    return ;
                }
                if (m_chunkedFlag == 0)
                {
                    if (isHex(tmp) == 0)
                    {
                        m_status = 400;
                        m_reqClose = 1;
                        return ;
                    }
                    len = std::stoi(tmp, 0, 16);
                    m_reqBodyLen = len;
                    m_chunkedFlag = 1;
                }
                else
                {
                    int len = tmp.length();
                    if (len - 2 != m_reqBodyLen)
                    {
                        m_status = 400;
                        m_reqClose = 1;
                        return ;
                    }
                    if (len == 2)
                    {
                        m_complete = 1;
                        m_end = 1;
                    }
                    m_body += tmp.substr(0, len - 2);
                    // std::cout << "m_body is " << m_body << std::endl;
                    m_readBodyLen += len - 2;
                    m_chunkedFlag = 0;
                }
                start += tmp.length();
            }
        }
        else if (m_reqBodyLen <= total_len - start)
        {
            m_body += m_buff.substr(start, m_reqBodyLen);
            m_remain = m_buff.substr(start + m_reqBodyLen, total_len - start - m_reqBodyLen);
            start = total_len;
            m_readBodyLen += m_reqBodyLen;
            m_reqBodyLen = 0;
            m_end = 1;
            m_complete = 1;
        }
        else
        {
            m_body += m_buff.substr(start, total_len - start);
            m_readBodyLen += total_len - start;
            m_reqBodyLen -= total_len - start;
        }
		// std::cout << m_readBodyLen << std::endl;
        if (m_readBodyLen > m_bodyLimit && m_status == 200)
        {
            m_status = 413;
            m_reqClose = 1;
        }
    }
    if (m_complete == 1 && start < total_len)
    {
        m_remain = m_buff.substr(start, total_len - start);
        if (m_remain.length() > 0)
        {
            // std::cout << "remain is " << m_remain << std::endl;
            // std::cout << "until now body is " << m_body << std::endl;
            // std::cout << "although one request is done, there is remain\n";
            // std::cout << "content length is " << m_contentLength << std::endl;
            // std::cout << "read body length is " << m_readBodyLen << std::endl;
        }
    }
}

void Request::PrintRequest() 
{
    std::cout << "Request " << "error:" << m_status << "{" << std::endl;
    std::cout << "Method : <" << m_method << ">" << std::endl;
    std::cout << "Path : <" << m_path << ">" << std::endl;
    std::cout << "Version : <" << m_version << ">" << std::endl;
    std::cout << "Headers{" << std::endl;
    for (std::multimap<std::string, std::string>::iterator it = m_headers.begin(); it != m_headers.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl;
    }
    // std::cout << "}" << std::endl;
    // std::cout << "Body : \n" << m_body << std::endl;
    // std::cout << "}" << std::endl;
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

void Request::checkEssential() 
{
    std::string tmp, hostport;
    if (m_headers.find("transfer-encoding") != m_headers.end() && m_headers.find("transfer-encoding")->second == "chunked")
        m_chunked = 1;
    if (m_method == "GET" || m_method == "HEAD")
    {
        if (m_headers.find("content-length") != m_headers.end())
        {
            m_status = 400;
            std::cout << "GET or HEAD method should not have content-length\n";
            m_reqBodyLen = 0;
            m_reqClose = 1;
        }
        else
        {
            m_reqBodyLen = 0;
            m_complete = 1;
            // std::cout << "\n\nGET or HEAD normal checked\n\n";
        }
        m_end = 1;
    }
    else if (m_headers.find("host") == m_headers.end() || m_headers.find("host")->second == "" || m_headers.count("host") > 1)
    {
        m_status = 400;
        std::cout << "Host header is missing or invalid\n";
        m_reqClose = 1;
        m_end = 1;
    }
    else if (m_chunked == 0)
    {
        if (m_headers.find("content-length") == m_headers.end())
        {
            m_status = 400;
            std::cout << "Content-Length header is missing\n";
            m_reqClose = 1;
            m_end = 1;
        }
        else
        {
            tmp = m_headers.find("content-length")->second;
            if (isNumber(m_headers.find("content-length")->second) == 0)
            {
                m_status = 411;
                std::cout << "Content-Length header is not a number\n";
                m_reqClose = 1;
                m_end = 1;
            }
            else
            {
                m_contentLength = atoi(tmp.c_str());
                m_reqBodyLen = m_contentLength;
                if (m_contentLength < 0 || m_contentLength > m_bodyLimit || m_headers.count("content-length") > 1)
                {
                    m_status = 400;
                    std::cout << "Content-Length header is invalid\n";
                    m_reqClose = 1;
                    m_end = 1;
                }
            }
        }
    }
    if (m_path.length() > 1024 && m_status == 200)
    {
        m_status = 414;
        std::cout << "URI is too long\n";
        m_reqClose = 1;
    }
    unsigned long pos;
    tmp = m_headers.find("host")->second;
    if (pos = tmp.find(":"), pos != std::string::npos)
    {
        m_host = tmp.substr(0, pos);
        hostport = tmp.substr(pos + 1, tmp.length() - pos - 1);
        if (isNumber(hostport) == 0)
        {
            m_status = 400;
            std::cout << "Port number is not a number\n";
            m_reqClose = 1;
            m_end = 1;
        }
        else
        {
            m_host_port = atoi(hostport.c_str());
            if (m_host_port < 0 || m_host_port > 65535 || errno == ERANGE)
            {
                m_status = 400;
                std::cout << "Port number is invalid\n";
                m_reqClose = 1;
                m_end = 1;
            }
        }
    }
    else
    {
        struct sockaddr_in addr;
        socklen_t         addr_len = sizeof(addr);
        if (getsockname(m_origin_fd, (struct sockaddr*)&addr, (socklen_t*)&addr_len) < 0)
        {
            m_status = 500;
            m_reqClose = 1;
            m_end = 1;
            return ;
        }
        m_host = tmp;
        m_host_port = ntohs(addr.sin_port);
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

int Request::isHex(const std::string& s) 
{
    for (unsigned long i = 0; i < s.length() - 2; i++)
    {
        if (!isdigit(s[i]) && (s[i] < 'A' || s[i] > 'F') && (s[i] < 'a' || s[i] > 'f'))
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

int Request::GetFd() 
{
    return (m_origin_fd);
}

int Request::IsComplete() 
{
    return (m_complete);
}

std::multimap<std::string, std::string>& Request::GetHeaders() 
{
    return (m_headers);
}

std::string& Request::GetSpecificHeader(const std::string& key) 
{
    if (m_headers.find(key) == m_headers.end())
        throw std::runtime_error("Header not found");
    return (m_headers.find(key)->second);
}

std::string& Request::GetMethod() 
{
    return (m_method);
}

std::string& Request::GetPath() 
{
    return (m_path);
}

std::string& Request::GetQuery() 
{
    return (m_query);
}

std::string& Request::GetVersion() 
{
    return (m_version);
}

std::string& Request::GetBody() 
{
    return (m_body);
}

std::string& Request::GetHost() 
{
    return (m_host);
}

int Request::GetPort() 
{
    return (m_host_port);
}

int Request::GetStatus() 
{
    return (m_status);
}

int Request::GetReqClose() 
{
    return (m_reqClose);
}

void Request::SetPath(std::string& path) 
{
	m_path = path;
}

std::string Request::GetHeader(const std::string& key) 
{
    if (m_headers.find(key) == m_headers.end())
        return ("");
    return (m_headers.find(key)->second);
}

std::string& Request::GetRemain() 
{
    return (m_remain);
}