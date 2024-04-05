#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <vector>

int main(int argc, char **argv, char **envp)
{
	int pipe_cgi[2];
	int pipe_cgi_output[2];
	pid_t pid;
	char buf[1025];

	pipe(pipe_cgi);
	pipe(pipe_cgi_output);
	pid = fork();
	if (pid == 0)
	{
		close(pipe_cgi[1]);
		close(pipe_cgi_output[0]);
		dup2(pipe_cgi[0], 0);
		dup2(pipe_cgi_output[1], 1);
		close(pipe_cgi[0]);
		close(pipe_cgi_output[1]);
		char *args[] = {"./cgi_tester", "./http/directory/youpi.bla", NULL};
		std::vector<std::string> env;
		for (int i = 0; envp[i]; i++)
			env.push_back(envp[i]);
		env.push_back("REQUEST_METHOD=POST");
		env.push_back("PATH_INFO=./http/directory/youpi.bla");
		env.push_back("SERVER_PROTOCOL=HTTP/1.1");

		char *envs[env.size() + 1];
		for (size_t i = 0; i < env.size(); i++)
			envs[i] = strdup(env[i].c_str());
		envs[env.size()] = NULL;
		execve(args[0], args, envs);
	}
	else if (pid > 0)
	{
		close(pipe_cgi[0]);
		close(pipe_cgi_output[1]);

		write(pipe_cgi[1], "aaaaaaa", 7);
		close(pipe_cgi[1]);
		int len = 0;
		while ((len = read(pipe_cgi_output[0], buf, 1024)) > 0)
		{
			buf[len] = '\0';
			std::cout << buf;
		}
		close(pipe_cgi_output[0]);
		waitpid(pid, NULL, 0);
	}


}