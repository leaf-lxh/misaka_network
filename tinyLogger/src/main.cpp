#include "logger.h"
#include "conf.h"

#include <iostream>

int main()
{
	TinyLogger service;
	std::cout << "Facilities amount: "  << service.AquireFacilities() << std::endl;

	service.StartListen();
	service.StartHandleRequest();

	return 0;
}
