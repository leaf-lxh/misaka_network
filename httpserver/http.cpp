#include "unp.h"
#include <fstream>
#include <sstream>
#include "./script/script.h"

// A function that read specific file,and store hole content to a given string buffer.
//path: specific file path
//buffer:a string varable that store file data.As string's max_size(), this function can only read the file that is smaller than 4GB.
//if function is successfully called,return 1,otherwise return 0.
int  ReadFile(const char *path,std::string &buffer)
{
	using std::fstream;
	fstream file(path,fstream::in) //read only

	if(!file.good())
	{
		return 0;
	}

	std::string temp = "";
	buffer = "";
	
	while(getline(file,temp))
	{
		buffer+=temp+"\n";
	}

	return 1;
}



