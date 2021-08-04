#include "remote.hpp"
#include <iostream>
void Datatrans::remotecontrol(std::string request)
{
	std::ofstream os;
	os.open("F:/gcrobot_server/api/robots/remote_command.json");
	std::cout << "open the command file" << std::endl;
	os << request;

	os.close();
	std::cout << "command download" << std::endl;
}

