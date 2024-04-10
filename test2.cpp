#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/event.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <set>
#include <string>
#include <iostream>


int main()
{
	std::set<std::string> s;


	s.insert("python");
	s.insert("hello");
	s.insert("hi");
	std::set<std::string>::iterator it = s.lower_bound("h");
	for (; it != s.end(); it++)
	{
		std::cout << *it << std::endl;
	}
}