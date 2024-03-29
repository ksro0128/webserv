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
    buf[bytes] = '\0';
    std::string buff(buf);
    if (bytes == 0)
    {
        doc.RemoveIncomplete(fd);
        if (close(fd) < 0)
            throw std::runtime_error("close error");
        std::cout << "Connection closed" << std::endl;
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
    if (doc.GetIncomplete(fd).IsComplete())
    {
        // doc.PutComplete(doc.GetIncomplete(fd));
        // doc.RemoveIncomplete(fd);
        // start test
        doc.PutDynamic(doc.GetIncomplete(fd));
        doc.RemoveIncomplete(fd);
        // end test
    }
}