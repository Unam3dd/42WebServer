#include "http_utils.hpp"
#include <iostream>

int main(int ac, char **av)
{
	if (ac < 2)
		return (1);
	std::cout << av[1] << std::endl;
	return (0);
}
