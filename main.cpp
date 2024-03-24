#include "main.hpp"

int main(int argc, char **argv)
{
	try
	{
		std::string configPath;
		if (argc == 1)
			configPath = "default.conf";
		else if (argc == 2)
			configPath = argv[1];
		else
		{
			std::cout << "Usage: " << argv[0] << " [config file path]" << std::endl;
			return 1;
		}
		
		WebServ webServ(configPath);
		webServ.runServer();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}