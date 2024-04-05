#include "FileUploaders.hpp"

FileUploaders::FileUploaders()
{
}

FileUploaders::~FileUploaders()
{
}

FileUploaders::FileUploaders(const FileUploaders &rhs)
{
    *this = rhs;
}

FileUploaders &FileUploaders::operator=(const FileUploaders &rhs)
{
    if (this != &rhs)
    {
    }
    return *this;
}

void FileUploaders::ProcessUpload(Document &doc)
{
    for (std::map<int, UploadInfo>::iterator it = doc.GetUploadFiles().begin(); it != doc.GetUploadFiles().end(); it++)
    {
        UploadInfo& info = it->second;
        if (info.isparsed == false)
        {
            info.boundary = getBoundary(info.req.GetHeader("content-type"));
            info.boundary_end = info.boundary + "--";
            // std::cout << "boundary: " << info.boundary << std::endl;
            parseFiles(info);
            // std::string filename = info.req.GetFilename();
            // std::string content_type = info.req.GetContentType();
            // std::string filedata = buffer.substr(buffer.find(boundary) + boundary.length());
            // filedata = filedata.substr(0, filedata.find(boundary));
            // std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);
            // file.write(filedata.c_str(), filedata.length());
            // file.close();
            if (info.isparsed == 0)
            {
                // std::cout << "file parsing error\n";
                close(info.req.GetFd());
                doc.GetFdEvent().erase(info.req.GetFd());
                continue;
            }
        }
    }
    doc.GetUploadFiles().clear();
}

std::string FileUploaders::getBoundary(const std::string &content_type)
{
    std::string boundary = "--" + content_type.substr(content_type.find("boundary=") + 9);
    return boundary;
}

void FileUploaders::parseFiles(UploadInfo &info)
{
    unsigned long start = 0;
    std::string& buffer = info.req.GetBody();
    // std::cout << "buffer: " << buffer << std::endl;
    while (1)
    {
        unsigned long pos = buffer.find(info.boundary, start);
        // std::cout << "pos: " << pos << std::endl;
        if (pos == std::string::npos || (pos = buffer.find(info.boundary_end, start)) == start)
            break;
        start += info.boundary.length() + 2;
        std::string line = getLine(start, buffer, "\n");
        if (line.find("filename=") == std::string::npos)
            continue;
        start += line.length();
        unsigned long subpos = line.find("filename=\"");
        unsigned long subpos2 = line.find("\"", subpos + 10);
        // std::cout << "hi\n";
        // std::cout << "line: " << line << std::endl;
        // std::cout << "subpos: " << subpos << std::endl;
        // std::cout << "subpos2: " << subpos2 << std::endl;
        std::string filename = line.substr(subpos + 10, subpos2 - subpos - 10);
        if (filename == "")
        {
            std::cout << "filename is empty\n";
            continue;
        }
        // std::cout << "dsjvcuihytwiuweuiteuwiteiwrhwqirqeqe\n";
        // std::cout << "filename: " << filename << std::endl;
        // std::cout << "dsafhksdafhlkadsf\n";
        std::string line2 = getLine(start, buffer, "\n");
        start += line2.length();
        std::string line3 = getLine(start, buffer, "\n");
        start += line3.length();
        unsigned long subpos3 = buffer.find("\n" + info.boundary, start);
        if (subpos3 == std::string::npos)
        {
            subpos3 = buffer.find("\n" + info.boundary_end, start);
            if (subpos3 == std::string::npos)
                return ;
        }
        std::string filedata = buffer.substr(start, subpos3 - start);
        start += filedata.length();
        info.filedata.push_back(std::pair<std::string, std::string>(filename, filedata));
        // std::cout << "filedata: " << filedata;
        // std::cout << "\n\nend\n\n";
    }
    info.isparsed = 1;
}

std::string FileUploaders::getLine(int start, std::string& s, const std::string& dilim) 
{
    unsigned long pos;
    std::string tmp;

    if ((pos = s.find(dilim, start)) != std::string::npos)
    {
        tmp = s.substr(start, pos - start + dilim.length());
        return (tmp);
    }
    else
        return (s.substr(start, s.length() - start));
}
