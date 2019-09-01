#include <map>
#include <string>
#include <vector>
#include <iostream>

class tester
{
public:
	/*
		Index for identifying tests, will be output by test functions.
		So if you want to identify tests, you should update index before you call the test functions.
	*/
	int index = 0;

	/*Pause function, equal to cin.get()*/
	void Pause();

	void OutputDebugString(std::initializer_list<std::string> message);
	void OutputDebugString(int indexNum, std::initializer_list<std::string> message);

	bool PrepareFile(std::string path, std::string content);
	int RemoveFile(std::string path);

	/*basic type comparing, using ==*/
	template <typename T>
	bool EXPECT_EQ(T expected, T input)
	{
		if (expected != input)
		{
			OutputDebugString(index, { "expected vs input uneuqal\n" });
			goto error_dump;
		}
		std::cout << "Index: " << index << " passed." << std::endl;
		return true;
	error_dump:
		OutputDebugString({ "=================================Input Context: =================================\n" });
		std::cout << input << std::endl;
		OutputDebugString({ "==================================================================================\n" });
		return false;
	}

	/*map*/
	template <typename mapT>
	bool EXPECT_EQ_MAP(mapT expected, mapT input)
	{
		if (expected != input)
		{
			OutputDebugString(index, { "map uneuqal\n" });
			goto error_dump;
		}
		std::cout << "Index: " << index << " passed." << std::endl;
		return true;
	error_dump:
		OutputDebugString({ "=================================Input Context: =================================\n" });
		for (auto it : input)
		{
			std::cout << it.first << ": " << it.second << std::endl;
		}
		OutputDebugString({ "==================================================================================\n" });
		return false;
	}
};
