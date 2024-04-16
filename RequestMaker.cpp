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
    if (bytes == 0)
    {
        doc.RemoveIncomplete(fd);
        if (close(fd) < 0)
            throw std::runtime_error("in byte 0 close error");
        std::cout << "Connection closed from fd " << fd << std::endl;
        doc.GetFdEvent().erase(fd);
        return;
    }
    else if (bytes < 0)
    {
        doc.RemoveIncomplete(fd);
        if (close(fd) < 0)
            throw std::runtime_error("in byte minus close error");
        std::cout << "read error from fd is " << fd << std::endl;
        return;
        // throw std::runtime_error("read error");
    }
    buf[bytes] = '\0';
    std::string buff(buf);
    std::cout << "read event from fd " << fd << std::endl;
    // std::cout << "read bytes from fd " << fd << " : " << bytes << std::endl;
    // std::cout << "read buf: " << buf << std::endl;
    doc.GetIncomplete(fd).ParseRequest(fd,buff);
    // if (doc.GetIncomplete(fd).IsComplete() || doc.GetIncomplete(fd).GetReqClose() == 1) // 헤더 파싱에러일 때, 왜 server not found 에러가 뜨는가????
    if (doc.GetIncomplete(fd).IsComplete())
    {
        // if (doc.GetIncomplete(fd).GetReqClose() == 1)
        // {
        //     doc.RemoveIncomplete(fd);
        //     std::cout << "get req close request is \n";
        //     doc.GetIncomplete(fd).PrintRequest();
        //     if (close(fd) < 0)
        //         throw std::runtime_error("in req close close error");
        //     std::cout << "Connection closed from fd " << fd << std::endl;
        //     doc.GetFdEvent().erase(fd);
        //     return;
        // }
        // doc.GetIncomplete(fd).PrintRequest();
        // return ;
        doc.PutComplete(doc.GetIncomplete(fd));
        doc.RemoveIncomplete(fd);
    }
}