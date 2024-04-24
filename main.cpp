#include "include.hpp"
#include "Webserv.hpp"

int main(int c, char **v, char **env)
{
    if (c > 2)
    {
        std::cout << "Usage: " << v[0] << " config file" << std::endl;
        return 1;
    }
	try
	{
		std::string path;
		if (c == 1)
			path = "./default/default.conf";
		else
			path = v[1];
		Webserv webserv(path, env);
		webserv.Run();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}