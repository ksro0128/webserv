#include "Classifier.hpp"

Classifier::Classifier()
{
}

void Classifier::Set(Config& config)
{
	m_config = config;
}

Classifier::~Classifier()
{
}

void Classifier::Classify(Document& document)
{
	std::vector<Request>&complete = document.GetComplete();
	for (std::vector<Request>::iterator it = complete.begin(); it != complete.end(); ++it)
	{
		classifyRequest(document, *it);
	}
	document.RemoveComplete();
}

void Classifier::classifyRequest(Document& document, Request &request)
{
	if (request.GetStatus() != 200)
	{
		document.PutStatic(request);
		return ;
	}
	Server &server = m_config.GetServer(request.GetPort(), request.GetHost());
	if (server.GetCgiFlag() == false)
	{
		document.PutStatic(request);
	}
	else
	{
		if (isCgi(request, server))
		{
			document.PutDynamic(request);
		}
		else
		{
			document.PutStatic(request);
		}
	}
}

bool Classifier::isCgi(Request &request, Server &server)
{
	std::string extension = getExtension(request.GetPath());
	std::cout << "extension : " << extension << std::endl;
	if (extension == "")
		return false;
	std::vector< std::vector<std::string> > cgiSet = server.GetCgi();
	for (std::vector< std::vector<std::string> >::iterator it = cgiSet.begin(); it != cgiSet.end(); ++it)
	{
		if (extension == (*it)[0])
			return true;
	}
	return false;
}

std::string Classifier::getExtension(std::string path)
{
	std::string extension;
	size_t pos = path.find_last_of(".");
	if (pos != std::string::npos)
	{
		extension = path.substr(pos + 1);
		pos = extension.find_first_of("/");
		if (pos != std::string::npos)
			extension.erase(pos);
	}
	else
		extension = "";
	return extension;
}