#pragma once

#include "../include.hpp"
#include "Location.hpp"

class Server
{
    public:
        Server();
        Server(const Server&);
        Server& operator=(const Server&);
        ~Server();
        void ParseServer(std::string block);
		void PrintServer();
		std::vector<int>& GetPort();
		std::vector<std::string>& GetServerName();
		std::string& GetRoot();
		std::vector<std::string>& GetIndexes();
		std::vector<std::string>& GetMethods();
		std::map<int, std::string>& GetErrorPages();
		int& GetLimitBodySize();
		std::vector<Location>& GetLocations();
		std::vector<Location>& GetCgi();
		bool GetPortFlag();
		bool GetServerNameFlag();
		bool GetRootFlag();
		bool GetIndexFlag();
		bool GetMethodFlag();
		bool GetLimitBodySizeFlag();
		bool GetLocationFlag();
		bool GetCgiFlag();

		Location& GetLocation(std::string path);
		bool IsCgi(std::string extension);
		Location& GetCgiLocation(std::string extension);
    private:
		std::vector<int> mPort;
		std::vector<std::string> mServerNames;
		std::string mRoot;
		std::vector<std::string> mIndexes;
		std::vector<std::string> mMethods;
		std::map<int, std::string> mErrorPages;
		int mLimitBodySize;
		std::vector<Location> mLocations;
		std::vector<Location> mCgi;

		bool mPortFlag;
		bool mServerNameFlag;
		bool mRootFlag;
		bool mIndexFlag;
		bool mMethodFlag;
		bool mLimitBodySizeFlag;
		bool mLocationFlag;
		bool mCgiFlag;

		void parsePort(std::istringstream& issLine);
		void parseServerName(std::istringstream& issLine);
		void parseRoot(std::istringstream& issLine);
		void parseIndex(std::istringstream& issLine);
		void parseMethod(std::istringstream& issLine);
		void parseErrorPage(std::istringstream& issLine);
		void parseLimitBodySize(std::istringstream& issLine);
		void parseLocation(std::istringstream& iss, std::istringstream& issLine);

		std::string getExtension(std::string path);
};