#pragma once

#include "include.hpp"

class Location
{
	public:
		Location();
		Location(const Location&);
		Location& operator=(const Location&);
		~Location();
		void ParseLocation(std::string block, std::string path);
		void SetLocation(std::string root, std::vector<std::string> index, std::vector<std::string> method);
		void PrintLocation();
		std::string& GetPath();
		std::string& GetRoot();
		std::vector<std::string>& GetIndex();
		std::vector<std::string>& GetMethod();
		bool& GetAutoIndex();
		int& GetRedirectionCode();
		std::string& GetRedirectionUri();
		
		bool& GetRedicetionFlag();
		
	private:
		std::string m_path;
		std::string m_root;
		std::vector<std::string> m_index;
		std::vector<std::string> m_method;
		bool m_autoIndex;
		int m_redirectionCode;
		std::string m_redirectionUri;

		bool m_rootFlag;
		bool m_indexFlag;
		bool m_methodFlag;
		bool m_autoIndexFlag;
		bool m_redirectionFlag;

		void parseRoot(std::istringstream& issLine);
		void parseIndex(std::istringstream& issLine);
		void parseMethod(std::istringstream& issLine);
		void parseAutoIndex(std::istringstream& issLine);
		void parseRedirection(std::istringstream& issLine);

};