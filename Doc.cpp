#include "Doc.hpp"

Doc::Doc()
{
    mRequest = NULL;
    mResponse = NULL;
    mStatus = 0;
    mFd = 0;
    mReadPipe = -1;
    mWritePipe = -1;
    mPid = -1;
    mCgiFlag = 0;
    mBuffer = "";
    mIndexInfo.start = 0;
    mIndexInfo.rest = 0;
}

Doc::~Doc()
{
    if (mRequest)
        delete mRequest;
    if (mResponse)
        delete mResponse;
}

Doc::Doc(const Doc &doc)
{
    if (doc.mRequest)
    {
        if (mRequest)
            delete mRequest;
        mRequest = new Request(*doc.mRequest);
    }
    if (doc.mResponse)
    {
        if (mResponse)
            delete mResponse;
        mResponse = new Response(*doc.mResponse);
    }
    mStatus = doc.mStatus;
    mFd = doc.mFd;
    mReadPipe = doc.mReadPipe;
    mWritePipe = doc.mWritePipe;
    mPid = doc.mPid;
    mCgiFlag = doc.mCgiFlag;
    mBuffer = doc.mBuffer;
    mIndexInfo.start = doc.mIndexInfo.start;
    mIndexInfo.rest = doc.mIndexInfo.rest;
}

void Doc::SetRequest(Request* request)
{
    mRequest = request;
}

void Doc::SetResponse(Response* response)
{
    mResponse = response;
}

void Doc::SetStatus(int status)
{
    mStatus = status;
}

void Doc::SetExecInfo(int readPipe, int writePipe, int pid)
{
    mReadPipe = readPipe;
    mWritePipe = writePipe;
    mPid = pid;
    mCgiFlag = 1;
    if (readPipe == -1 && writePipe == -1 && pid == -1)
        mCgiFlag = 0;
}

Request* Doc::GetRequest()
{
    return mRequest;
}

void Doc::RemoveRequest()
{
    if (mRequest)
    {
        delete mRequest;
        mRequest = NULL;
    }
}

Response* Doc::GetResponse()
{
    return mResponse;
}

void Doc::RemoveResponse()
{
    if (mResponse)
    {
        delete mResponse;
        mResponse = NULL;
    }
}

int Doc::GetStatus()
{
    return mStatus;
}

int Doc::GetFd()
{
    return mFd;
}

void Doc::SetFd(int fd)
{
    mFd = fd;
}

int Doc::GetReadPipe()
{
    return mReadPipe;
}

int Doc::GetWritePipe()
{
    return mWritePipe;
}

int Doc::GetPid()
{
    return mPid;
}

int Doc::GetCgiFlag()
{
    return mCgiFlag;
}

std::string& Doc::GetBuffer()
{
    return mBuffer;
}

void Doc::SetIndexInfo(int start, int rest)
{
    mIndexInfo.start = start;
    mIndexInfo.rest = rest;
    mCgiFlag = 1;
}

IndexInfo& Doc::GetIndexInfo()
{
    return mIndexInfo;
}