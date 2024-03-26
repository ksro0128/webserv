#include "ResponseSender.hpp"

ResponseSender::ResponseSender()
{
}

void ResponseSender::Set(Config &config, int kq)
{
	m_config = config;
	m_kq = kq;
}

ResponseSender::~ResponseSender()
{
}

void ResponseSender::SendResponses(Document &document)
{
	std::vector<Response> &responses = document.GetResponse();
	for (std::vector<Response>::iterator it = responses.begin(); it != responses.end(); it++)
	{
		Response response = *it;
		int clientSock = response.GetOriginFd();
		std::cout << "clientSock: " << clientSock << std::endl;
		std::string responseStr = response.GetResponse();
		std::cout << "responseStr: " << responseStr << std::endl;
		int len = write(clientSock, responseStr.c_str(), responseStr.length());
		if (len == -1 || static_cast<unsigned long>(len) != responseStr.length())
		{
			std::cout << "write error" << std::endl;
		}
		// write(clientSock, responseStr.c_str(), responseStr.length());
		close(clientSock);
	}
	responses.clear();
}