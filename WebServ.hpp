#pragma once

#include "main.hpp"
#include "Document.hpp"
#include "RequestMaker.hpp"
#include "RequestProcessor.hpp"
#include "StaticProcessor.hpp"
#include "ResponseSender.hpp"
#include "CgiProcessor.hpp"
#include "Classifier.hpp"
#include "FileUploaders.hpp"

class WebServ
{
	public:
		WebServ(std::string configPath);
		~WebServ();
		void RunServer();
	private:
		WebServ();
		WebServ(const WebServ&);
		WebServ& operator=(const WebServ&);
		Document m_document;
		RequestMaker m_requestMaker;
		RequestProcessor m_requestProcessor;
		ResponseSender m_responseSender;
		CgiProcessor m_cgiProcessor;
		Classifier m_classifier;
		StaticProcessor m_staticProcessor;
		FileUploaders m_fileUploaders;
		Config m_config;
		std::vector<int> m_servSocks;
		std::vector<int> m_ports;
		int m_kq;
		int openPort(int port);
};

///규칙
// public 함수 앞 대문자시작 private 함수 앞 소문자시작
// 멤버변수는 m_으로 시작

// 전부다 중간 시작점은 대문자