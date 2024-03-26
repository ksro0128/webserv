#ifndef REQUESTMAKER_HPP
# define REQUESTMAKER_HPP

# include "include.hpp"
# include "Document.hpp"

class RequestMaker{
public:
    RequestMaker();
    ~RequestMaker();
    RequestMaker(const RequestMaker &rhs);
    RequestMaker &operator=(const RequestMaker &rhs);
    void makeRequest(Document& doc, int fd);
};

#endif