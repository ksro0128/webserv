#ifndef FILEUPLOADERS_HPP
# define FILEUPLOADERS_HPP

#include "include.hpp"
#include "Request.hpp"
#include "Document.hpp"
#include "Response.hpp"

class FileUploaders
{
public:
    FileUploaders();
    ~FileUploaders();
    FileUploaders(const FileUploaders& r);
    FileUploaders& operator=(const FileUploaders& r);

    void ProcessUpload(Document& doc);
private:
    std::string getBoundary(const std::string& content_type);
    void parseFiles(UploadInfo& info);
    std::string getLine(int start, std::string& buffer, const std::string& dilim);
};

#endif