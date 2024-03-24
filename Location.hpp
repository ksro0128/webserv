#pragma once

#include "include.hpp"

class Location
{
	public:
		Location();
		Location(const Location&);
		Location& operator=(const Location&);
		~Location();
		void parseLocation(std::string block, std::string path);
		void setLocation(std::string root, std::vector<std::string> index, std::vector<std::string> method);
		void printLocation();
		std::string& getPath();
		std::string& getRoot();
		std::vector<std::string>& getIndex();
		std::vector<std::string>& getMethod();
		bool& getAutoIndex();
		int& getRedirectionCode();
		std::string& getRedirectionUri();
		
		bool& getRedicetionFlag();
		
	private:
		std::string _path;
		std::string _root;
		std::vector<std::string> _index;
		std::vector<std::string> _method;
		bool _autoIndex;
		int _redirectionCode;
		std::string _redirectionUri;

		bool _rootFlag;
		bool _indexFlag;
		bool _methodFlag;
		bool _autoIndexFlag;
		bool _redirectionFlag;

		void _parseRoot(std::istringstream& issLine);
		void _parseIndex(std::istringstream& issLine);
		void _parseMethod(std::istringstream& issLine);
		void _parseAutoIndex(std::istringstream& issLine);
		void _parseRedirection(std::istringstream& issLine);

};