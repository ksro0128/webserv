#pragma once

#include "include.hpp"
#include "Config.hpp"
#include "Response.hpp"
#include "Document.hpp"


class ResponseSender
{
	public:
		ResponseSender();
		~ResponseSender();
		void Set(Config &config, int kq);
		void SendResponses(Document &document, int fd);
	private:
		ResponseSender(const ResponseSender &rhs);
		ResponseSender &operator=(const ResponseSender &rhs);

		Config m_config;
		int m_kq;
};