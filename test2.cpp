#include <iostream>
#include <string>
#include <map>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>

int main()
{
    std::string a = "c0";
    std::cout << strtol(a.c_str(), NULL, 16) << std::endl;
    // std::string start = "<html>\n<head><title>Index of /</title></head>\n<body>\n<h1>Index of /</h1><hr><pre>";
    // std::string end = "</pre><hr></body>\n</html>";
    // std::string before = "<a href=\"";
    // std::string after = "\">";
    // std::string folder = "./";
    // timespec  date;
    // long size;
    // std::string size_str;
    // std::string info;
    // DIR* dp = opendir(folder.c_str());
    // struct stat st;
    // struct dirent* dir;
    // struct tm *nowtm;
    // char tmbuf[64], buf[64];
    // std::string whitespace;
    // int fd = open("index.html", O_CREAT | O_RDWR | O_TRUNC, 0644);
    // write(fd, start.c_str(), start.size());
    // while ((dir = readdir(dp)) != NULL)
    // {
    //     whitespace = "";
    //     if (dir->d_name[0] == '.' && dir->d_name[1] == '\0')
    //         continue;
    //     std::string name = dir->d_name;
    //     stat((folder + name).c_str(), &st);
    //     if (S_ISDIR(st.st_mode))
    //     {
    //         date = st.st_mtimespec;
    //         int len = name.length();
    //         for (int i = 30; i > len; i--)
    //             whitespace += " ";
    //         write(fd, before.c_str(), before.size());
    //         write(fd, name.c_str(), name.size());
    //         write(fd, after.c_str(), after.size());
    //         info = name + "</a>" + whitespace + '-' + "\n";
    //         write(fd, info.c_str(), info.size());
    //     }
    // }
    // closedir(dp);
    // dp = opendir(folder.c_str());
    // while ((dir = readdir(dp)) != NULL)
    // {
    //     whitespace = "";
    //     if (dir->d_name[0] == '.')
    //         continue;
    //     std::string name = dir->d_name;
    //     stat((folder + name).c_str(), &st);
    //     if (!S_ISDIR(st.st_mode))
    //     {
    //         date = st.st_mtimespec;
    //         int len = name.length();
    //         for (int i = 30; i > len; i--)
    //             whitespace += " ";
    //         nowtm = localtime(&date.tv_sec);
    //         strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
    //         std::string date_str = tmbuf;
    //         size = st.st_size;
    //         if (size > 1024)
    //             size_str = std::to_string(size / 1024) + "K";
    //         else
    //             size_str = std::to_string(size) + "B";
    //         write(fd, before.c_str(), before.size());
    //         write(fd, name.c_str(), name.size());
    //         write(fd, after.c_str(), after.size());
    //         info = name + "</a>" + whitespace + size_str + "\t" + tmbuf + "\n";
    //         write(fd, info.c_str(), info.size());
    //     }
    // }
    // write(fd, end.c_str(), end.size());
    // closedir(dp);
    // close(fd);
    return 0;
}


1234567890

123 에서 끝났고 45가 \r\n이면
67890이 remain에 저장되어야함
