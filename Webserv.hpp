#pragma once

#include "include.hpp"
#include "./Config/Config.hpp"
#include "Request.hpp"
#include "Doc.hpp"


#include "Response.hpp"

#define SERVER 1
#define CLIENT 2
#define PIPE 3
#define PID 4

#define DYNAMIC 1
#define STATIC 2

class Webserv
{
    public:
		Webserv(std::string path, char **env);
        ~Webserv();
        void Run();
    private:
        int mKq;
        Webserv();
        Config mConfig;
        std::map<int, Doc*> mDoc;
        std::map<int, int> mFid[5];
        std::map<int, int> mType;
        std::vector<std::string> mEnv;
        std::vector<int> mPorts;
        std::vector<int> mServSocks;

        ////////////open port////////////
        int openPort(int port);
        /////////////////////////////////


        ////////////parse request part///////////
        int parseRequest(int fd, Doc* document);
        int readParse(int fd, Request* request);
        /////////////////////////////////////////

        ////////////Set Event Util//////////////
        void setEvent(int fd, int filter, int flag, int fflag);
        void setTimer(int fd, int time, int flag);
        ///////////////////////////////////////

        ////////////Processing Request Part////////
        void processRequest(Doc* document);
        // int  checkRedirection(Doc* document); // 리다이렉션 2가지 처리
        void processGET(Doc* document);
        void processDELETE(Doc* document);
        void processStaticFile(Doc* document, const std::string& filepath);
        std::string makeAutoIndex(const std::string& filepath);

        ////////////making response part///////////
        void makeResponse(Doc* document, int status, const std::string& msg, const std::string& body);
        ///////////////////////////////////////////

        ////////////send response part/////////////
        void sendResponse(int fd, Doc* document);
        ///////////////////////////////////////////

        //////////////////CGI Part///////////////
        void excuteCGI(Doc* document);
        void readPipe(int fd, Doc* document);
        void writePipe(int fd, Doc* document);
        void inChild(Doc* document, std::string& path, std::string& cgi_path, int p1[2], int p2[2]);
        void waitProcess(int pid, Doc* document);
        void parseCgiBody(Doc* document, std::string& body);
        void addEnv(const std::string& key, const std::string& value);
        std::string intToString(int num);
        /////////////////////////////////////////


        ////////////temporary setting responsee error////////
        void setResponseError(Doc* document, int status);
        /////////////////////////////////////////////////////

        ////////////register client fd////////////
        void registerClient(int fd, Doc* document);
        /////////////////////////////////////////


        void processStatic(Doc *document);
        int checkBodySize(Doc *document);
        int checkRequestError(Doc *document);
        int checkMethod(Doc *document);
        int checkRedirection(Doc *document);
        int checkIndex(Doc *document);
        int checkCgi(Doc *document);
        void processGet(Doc *document);
        void processPost(Doc *document);
        void processDelete(Doc *document);


        void deleteCgiInfo(Doc *document);


};