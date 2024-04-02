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
    int bytes = 0;
    
    bytes = read(fd, buf, 1024);
    std::string buff(buf);
    if (bytes == 0)
    {
        doc.RemoveIncomplete(fd);
        if (close(fd) < 0)
            throw std::runtime_error("close error");
        std::cout << "Connection closed" << std::endl;
        doc.GetFdEvent().erase(fd);
        return;
    }
    else if (bytes < 0)
    {
        doc.RemoveIncomplete(fd);
        if (close(fd) < 0)
            throw std::runtime_error("close error");
        std::cout << "read error" << std::endl;
        throw std::runtime_error("read error");
    }
    doc.GetIncomplete(fd).ParseRequest(fd,buff);
    // if (doc.GetIncomplete(fd).IsComplete() || doc.GetIncomplete(fd).GetReqClose() == 1) // 헤더 파싱에러일 때, 왜 server not found 에러가 뜨는가????
    if (doc.GetIncomplete(fd).IsComplete())
    {
        // doc.GetIncomplete(fd).PrintRequest();
        doc.PutComplete(doc.GetIncomplete(fd));
        doc.RemoveIncomplete(fd);
        // start test
        // doc.PutDynamic(doc.GetIncomplete(fd));
        doc.RemoveIncomplete(fd);
        // std::cout << "complete making request and put to dynamic" << std::endl;
        // end test
    }
}