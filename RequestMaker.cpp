#include "RequestMaker.hpp"

RequestMaker::RequestMaker()
{
}

RequestMaker::~RequestMaker()
{
}

RequestMaker::RequestMaker(const RequestMaker &rhs)
{
    *this = rhs;
}

RequestMaker &RequestMaker::operator=(const RequestMaker &rhs)
{
    if (this != &rhs)
    {
    }
    return *this;
}

void RequestMaker::makeRequest(Document &doc, int fd)
{
    char buf[1025];
    int bytes;
    bytes = read(fd, buf, 1024);
    buf[bytes] = '\0';
    // std::cout << "read bytes from fd " << fd << " : " << bytes << std::endl;
    // std::cout << "read buf: " << buf << std::endl;
    std::string buff(buf);
    if (bytes == 0)
    {
        doc.RemoveIncomplete(fd);
        if (close(fd) < 0)
            throw std::runtime_error("in byte 0 close error");
        std::cout << "Connection closed" << std::endl;
        doc.GetFdEvent().erase(fd);
        return;
    }
    else if (bytes < 0)
    {
        doc.RemoveIncomplete(fd);
        if (close(fd) < 0)
            throw std::runtime_error("in byte minus close error");
        std::cout << "read error" << std::endl;
        throw std::runtime_error("read error");
    }
    doc.GetIncomplete(fd).ParseRequest(fd,buff);
    if (doc.GetIncomplete(fd).IsComplete() || doc.GetIncomplete(fd).GetReqClose() == 1) // 헤더 파싱에러일 때, 왜 server not found 에러가 뜨는가????
    // if (doc.GetIncomplete(fd).IsComplete())
    {
        // doc.GetIncomplete(fd).PrintRequest();
        doc.PutComplete(doc.GetIncomplete(fd));
        doc.RemoveIncomplete(fd);
    }
}