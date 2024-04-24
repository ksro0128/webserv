#pragma once

#include "../include.hpp"

class Location
{
	public:
		Location();
		Location(const Location&);
		Location& operator=(const Location&);
		~Location();
		void ParseLocation(std::string block, std::string path);
		void PrintLocation();


		std::string& GetPath();
		std::string& GetRoot();
		std::vector<std::string>& GetIndexes();
		std::vector<std::string>& GetMethods();
		bool& GetAutoIndex();
		int GetRedirectionCode();
		std::string& GetRedirectionPath();
		std::string& GetCgiPath();
		int& GetLimitBodySize();
		bool GetRootFlag();
		bool GetIndexFlag();
		bool GetMethodFlag();
		bool GetAutoIndexFlag();
		bool GetRedirectionFlag();
		bool GetCgiPathFlag();
		bool GetLimitBodySizeFlag();

	
	private:
		std::string mPath;
		std::string mRoot;
		std::vector<std::string> mIndexes;
		std::vector<std::string> mMethods;
		bool mAutoIndex;
		int mRedirectionCode;
		std::string mRedirectionPath;
		std::string mCgiPath;
		int mLimitBodySize;

		bool mRootFlag;
		bool mIndexFlag;
		bool mMethodFlag;
		bool mAutoIndexFlag;
		bool mRedirectionFlag;
		bool mCgiPathFlag;
		bool mLimitBodySizeFlag;

		void parseRoot(std::istringstream& issLine);
		void parseIndex(std::istringstream& issLine);
		void parseMethod(std::istringstream& issLine);
		void parseAutoIndex(std::istringstream& issLine);
		void parseRedirection(std::istringstream& issLine);
		void parseCgiPath(std::istringstream& issLine);
		void parseLimitBodySize(std::istringstream& issLine);
};