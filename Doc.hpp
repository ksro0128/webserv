#pragma once

#include "include.hpp"
#include "Request.hpp"
#include "Response.hpp"

typedef struct s_IndexInfo{
    int     start;
    int     rest;
}IndexInfo;

class Doc
{
    public:
        Doc();
        ~Doc();
        Doc(const Doc &doc);

        void SetRequest(Request* request);
        void SetResponse(Response* response);
        void SetStatus(int status);
        void SetExecInfo(int readPipe, int writePipe, int pid);

        Request*        GetRequest();
        void            RemoveRequest();

        Response*       GetResponse();
        void            RemoveResponse();

        int             GetStatus();
        int             SetStatus();

        int             GetFd();
        void            SetFd(int fd);

        int             GetReadPipe();
        int             GetWritePipe();


        int             GetPid();
        int             GetCgiFlag();
        std::string&    GetBuffer();

        IndexInfo&      GetIndexInfo();
        void            SetIndexInfo(int start, int rest);
    private:
        Doc& operator=(const Doc &doc);
        Request*    mRequest;    //동적으로 한 이유 : 정적으로 하면 set으로 해줘야해서
        Response*   mResponse;
        int         mStatus; // 0=free 1=busy
        int         mCgiFlag;
        int         mFd;
        int         mReadPipe;
        int         mWritePipe;
        int         mPid;
        IndexInfo   mIndexInfo;
        std::string mBuffer;
};