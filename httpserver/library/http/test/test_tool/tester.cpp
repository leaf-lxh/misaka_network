#include "tester.h"


#include <initializer_list>
#include <fstream>
#include <unistd.h>

void tester::Pause()
{
	std::cout << "Press Enter to continue." << std::endl;
	std::cin.get();
}

void tester::OutputDebugString(std::initializer_list<std::string> message)
{
	for (auto it : message)
	{
		std::cout << it << std::flush;
	}
}

void tester::OutputDebugString(int indexNum, std::initializer_list<std::string> message)
{
	std::cout << "Index " << indexNum << ": " << std::flush;
	for (auto it : message)
	{
		std::cout << it << std::flush;
	}
}

bool tester::PrepareFile(std::string path, std::string content)
{
	std::fstream file(path, std::fstream::binary | std::fstream::out);
	if (file.good() == false)
	{
		return false;
	}

	file.write(content.c_str(), content.length());
	return true;

}
int tester::RemoveFile(std::string path)
{
	return unlink(path.c_str());
}

