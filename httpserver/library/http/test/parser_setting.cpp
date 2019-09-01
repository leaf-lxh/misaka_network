#include "../parser/setting.h"

#include "test_tool/tester.h"
#include <iostream>

int main()
{
	parser::SettingReader reader;
	tester t;

	t.PrepareFile("setting.conf", "name=test\nage=200\nlevel = 5\ncomment=test #this is comment\n");
	t.index = 1;


	t.EXPECT_EQ_MAP(std::multimap<std::string, std::string>({ {"age", "200"},{"comment", "test"},{"level", "5"},{"name", "test"} }), reader.RetriveFromKeyValueFmt("setting.conf"));

	t.RemoveFile("setting.conf");

}
