#include "WebServ.hpp"

WebServ::WebServ()
{
}

WebServ::WebServ(std::string configPath)
{
	_config.parseConfig(configPath);
}

WebServ::~WebServ()
{
}

void WebServ::runServer()
{
	std::vector<int> servSocks;
	std::vector<int> ports = _config.getPorts();
	int kq = kqueue();
	if (kq == -1)
	{
		throw std::runtime_error("kqueue() error");
	}
	struct kevent ev_set;
	struct kevent ev_list[50];
	
	for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); ++it)
	{
		servSocks.push_back(openPort(*it));
		EV_SET(&ev_set, servSocks.back(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		kevent(kq, &ev_set, 1, NULL, 0, NULL);
	}

	while (1)
	{
		int nev = kevent(kq, NULL, 0, ev_list, 50, NULL);
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
				if (std::find(servSocks.begin(), servSocks.end(), ev_list[i].ident) != servSocks.end()) // 서버 소켓 - 새로운 연결
				{
					struct sockaddr_in clnt_adr;
					socklen_t adr_sz = sizeof(clnt_adr);
					int clntSock = accept(ev_list[i].ident, (struct sockaddr *)&clnt_adr, &adr_sz);
					int flags = fcntl(clntSock, F_GETFL, 0);
					flags |= O_NONBLOCK;
					fcntl(clntSock, F_SETFL, flags);
					EV_SET(&ev_set, clntSock, EVFILT_READ, EV_ADD, 0, 0, NULL);
					kevent(kq, &ev_set, 1, NULL, 0, NULL);
					std::cout << "new connection" << std::endl;
				}
				else // 클라이언트 소켓 - 요청 처리
				{
					// request 파싱
					char buffer[1024];
					int len = read(ev_list[i].ident, buffer, 1024);
					if (len == -1)
					{
						throw std::runtime_error("read() error");
					}
					buffer[len] = '\0';
					if (len == 0)
					{
						close(ev_list[i].ident);
						std::cout << "connection closed" << std::endl;
						continue;
					}
					std::cout << buffer << std::endl;
					
				}
			}
			else if (ev_list[i].filter == EVFILT_WRITE) // write 이벤트
			{
				// response 보내기		
			}
		}
		// vector 받아서 실행 -> 실행상태 반환

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