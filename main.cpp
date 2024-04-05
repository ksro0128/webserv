#include "main.hpp"

int main(int argc, char **argv, char **envp)
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
		
		std::vector<std::string> env;
		for (int i = 0; envp[i]; i++)
			env.push_back(envp[i]);
		WebServ webServ(configPath, env);
		webServ.RunServer();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}