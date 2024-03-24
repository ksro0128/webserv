#pragma once

#include "include.hpp"
#include "Location.hpp"

class Server
{
    public:
        Server();
        Server(const Server&);
        Server& operator=(const Server&);
        ~Server();
        void parseServer(std::string block);
		std::vector<int>& getPort();
		std::vector<std::string>& getServerName();
		std::string& getRoot();
		std::vector<std::string>& getIndex();
		std::vector<std::string>& getMethod();
		std::map<int, std::string>& getErrorPage();
		int& getLimitBodySize();
		std::vector<Location>& getLocation();
		std::vector< std::vector<std::string> >& getCgi();
		bool& getCgiFlag();

		void printServer();
    private:
		std::vector<int> _port;
		std::vector<std::string> _serverName;
		std::string _root;
		std::vector<std::string> _index;
		std::vector<std::string> _method;
		std::map<int, std::string> _errorPage;
		int _limitBodySize;
		std::vector< std::vector<std::string> > _cgi;
		std::vector<Location> _location;

		bool _portFlag; 
		bool _serverNameFlag;
		bool _rootFlag;
		bool _indexFlag;
		bool _methodFlag;
		bool _errorPageFlag;
		bool _limitBodySizeFlag;
		bool _cgiFlag;
		bool _locationFlag;

		void _parsePort(std::istringstream& issLine);
		void _parseServerName(std::istringstream& issLine);
		void _parseRoot(std::istringstream& issLine);
		void _parseIndex(std::istringstream& issLine);
		void _parseMethod(std::istringstream& issLine);
		void _parseErrorPage(std::istringstream& issLine);
		void _parseLimitBodySize(std::istringstream& issLine);
		void _parseCgi(std::istringstream& issLine);
		void _parseLocation(std::istringstream& iss, std::istringstream& issLine);
};