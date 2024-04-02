#include "WebServ.hpp"

WebServ::WebServ(std::string configPath)
{
	m_config.ParseConfig(configPath);

	m_kq = kqueue();
	m_requestProcessor.Set(m_config, m_kq);
	m_responseSender.Set(m_config, m_kq);
	m_cgiProcessor.Set(m_config, m_kq);
	m_classifier.Set(m_config);
	m_staticProcessor.Set(m_config, m_kq);
	if (m_kq == -1)
	{
		throw std::runtime_error("kqueue() error");
	}
	m_servSocks.clear();
	m_ports = m_config.GetPorts();

	struct kevent ev_set;
	for (std::vector<int>::iterator it = m_ports.begin(); it != m_ports.end(); ++it)
	{
		m_servSocks.push_back(openPort(*it));
		EV_SET(&ev_set, m_servSocks.back(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		kevent(m_kq, &ev_set, 1, NULL, 0, NULL);
		m_document.PutFdEvent(m_servSocks.back(), "server");
	}
}

WebServ::~WebServ()
{
}

void WebServ::RunServer()
{
	struct kevent ev_set;
	struct kevent ev_list[50];

	while (1)
	{
		int nev = kevent(m_kq, NULL, 0, ev_list, 50, NULL);
		for (int i = 0; i < nev; i++)
		{
			// if (ev_list[i].filter == EVFILT_READ && ev_list[i].flags & EV_EOF) // 연결 종료
			// {
			// 	close(ev_list[i].ident);
			// 	std::cout << "connection closed" << std::endl;
			// 	continue;
			// }
			// else 
			if (ev_list[i].filter == EVFILT_READ) // read 이벤트
			{
				if (std::find(m_servSocks.begin(), m_servSocks.end(), ev_list[i].ident) != m_servSocks.end()) // 서버 소켓 - 새로운 연결
				{
					struct sockaddr_in clnt_adr;
					socklen_t adr_sz = sizeof(clnt_adr);
					int clntSock = accept(ev_list[i].ident, (struct sockaddr *)&clnt_adr, &adr_sz);
					int flags = fcntl(clntSock, F_GETFL, 0);
					flags |= O_NONBLOCK;
					fcntl(clntSock, F_SETFL, flags);
					EV_SET(&ev_set, clntSock, EVFILT_READ, EV_ADD, 0, 0, NULL);
					kevent(m_kq, &ev_set, 1, NULL, 0, NULL);
					m_document.PutFdEvent(clntSock, "client");
					std::cout << "new connection" << std::endl;
				}
				else if (m_document.GetExcute().find(ev_list[i].ident) != m_document.GetExcute().end()) // cgi 소켓 - 응답 처리
				{
					// std::cout << "read event for cgi" << std::endl;
					// m_cgiProcessor.Read(m_document, ev_list[i].ident);
				}
				else // 클라이언트 소켓 - 요청 처리
				{
					m_requestMaker.makeRequest(m_document, ev_list[i].ident);
				}
				
			}
			else if (ev_list[i].filter == EVFILT_WRITE) // write 이벤트
			{
				// response 보내기
				m_responseSender.SendResponses(m_document);
			}
			else // process 이벤트
			{
				//wait 해야함
				// response 보내기
				// std::cout << "process wait and making response event" << std::endl;
				// m_cgiProcessor.Wait(m_document, ev_list[i].ident);
			}
		}
		m_classifier.Classify(m_document);
		m_staticProcessor.Process(m_document);
		m_document.ClearDynamic();
		// std::cout << "static" << std::endl;
		// std::vector<Request>& requests_s = m_document.GetStatic();
		// for (std::vector<Request>::iterator it = requests_s.begin(); it != requests_s.end(); it++)
		// {
		// 	it->PrintRequest();
		// }
		// std::vector<Request>& requests_d = m_document.GetDynamic();

		// std::cout << "dynamic" << std::endl;
		// for (std::vector<Request>::iterator it = requests_d.begin(); it != requests_d.end(); it++)
		// {
		// 	it->PrintRequest();
		// }
		// m_document.ClearStatic();
		// m_document.ClearDynamic();
		// vector 받아서 실행 -> 실행상태 반환
		// m_requestProcessor.ProcessRequests(m_document);
		// m_cgiProcessor.ExcuteCgi(m_document);
		//wait 해야함
		// response 보내기
	}

	
}

int WebServ::openPort(int port)
{
	int servSock;
	struct sockaddr_in servAdr;
	
	servSock = socket(PF_INET, SOCK_STREAM, 0);
	if (servSock == -1)
	{
		throw std::runtime_error("socket() error");
	}
	int flags = fcntl(servSock, F_GETFL, 0);
    if (flags == -1)
	{
		throw std::runtime_error("fcntl() error");
	}
	flags |= O_NONBLOCK;
    fcntl(servSock, F_SETFL, flags);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(port);
	int	option = 1;
	setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option) );
	if (bind(servSock, (struct sockaddr *)&servAdr, sizeof(servAdr)) == -1)
	{
		throw std::runtime_error("bind() error");
	}
	if (listen(servSock, 5) == -1)
	{
		throw std::runtime_error("listen() error");
	}
	return servSock;
}