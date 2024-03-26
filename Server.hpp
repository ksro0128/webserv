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
        void ParseServer(std::string block);
		std::vector<int>& GetPort();
		std::vector<std::string>& GetServerName();
		std::string& GetRoot();
		std::vector<std::string>& GetIndex();
		std::vector<std::string>& GetMethod();
		std::map<int, std::string>& GetErrorPage();
		int& GetLimitBodySize();
		std::vector<Location>& GetLocation();
		std::vector< std::vector<std::string> >& GetCgi();
		bool& GetCgiFlag();

		void PrintServer();
    private:
		std::vector<int> m_port;
		std::vector<std::string> m_serverName;
		std::string m_root;
		std::vector<std::string> m_index;
		std::vector<std::string> m_method;
		std::map<int, std::string> m_errorPage;
		int m_limitBodySize;
		std::vector< std::vector<std::string> > m_cgi;
		std::vector<Location> m_location;

		bool m_portFlag; 
		bool m_serverNameFlag;
		bool m_rootFlag;
		bool m_indexFlag;
		bool m_methodFlag;
		bool m_errorPageFlag;
		bool m_limitBodySizeFlag;
		bool m_cgiFlag;
		bool m_locationFlag;

		void parsePort(std::istringstream& issLine);
		void parseServerName(std::istringstream& issLine);
		void parseRoot(std::istringstream& issLine);
		void parseIndex(std::istringstream& issLine);
		void parseMethod(std::istringstream& issLine);
		void parseErrorPage(std::istringstream& issLine);
		void parseLimitBodySize(std::istringstream& issLine);
		void parseCgi(std::istringstream& issLine);
		void parseLocation(std::istringstream& iss, std::istringstream& issLine);
};