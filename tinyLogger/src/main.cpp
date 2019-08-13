#include "logger.h"
#include "conf.h"

#include <iostream>
#include <string>
#include <cstring>
#include <set>
#include <memory>
#include <fstream>

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <signal.h>


#define TINYLOGGER_SERVER_PID_DIR "/var/run/tinyLogger/"

#define TINYLOGGER_SERVER_PID_FILE "/var/run/tinyLogger/master.pid"

char **environ;
char **argv;

std::unique_ptr<char[]> newEnviron;
size_t regionSize;

TinyLogger server;
std::set<int> workerPID;
bool worker = false;

void SetProcessTitleInit()
{
	regionSize = 0;
	for (int i = 0; environ[i]; ++i)
	{
		regionSize += strlen(environ[i]) + 1;
	}
	newEnviron.reset(new char[regionSize]());

	char *argv_last = argv[0];
	for (int i = 0; argv[i]; ++i)
	{
		if (argv_last == argv[i])
		{
			argv_last += strlen(argv[i]) + 1;
		}
	}

	char *pEnviron = newEnviron.get();
	for (int i = 0; environ[i]; ++i)
	{
		if (argv_last == environ[i])
		{
			int size = strlen(environ[i]) + 1;
			strncpy(pEnviron, environ[i], size);
			environ[i] = pEnviron;

			pEnviron += size;
			argv_last += size;
		}
	}
}

void SetProcessTitle(const char* title)
{
	std::string processTitle = "tinyLogger: ";
	processTitle += title;

	/* The way you are avoiding buffer overflow, like cxk */
	size_t writeSize = (processTitle.length() >= regionSize) ? regionSize - 1 : processTitle.length() + 1;
	strncpy(argv[0], processTitle.c_str(), writeSize);
	std::cout << argv[0] << std::endl;
}


int ArgumentParser(char **argv)
{
	char **parg = argv;
	++parg;
	while(*parg)
	{
		std::string argString(*parg);
		if (argString == "start")
		{
			return 1;
		}
		else if (argString == "status")
		{
			return 2;
		}
		else if (argString == "stop")
		{
			return 3;
		}
		else
		{
			return -1;
		}
	}
}

void CreateMasterPIDFile()
{
	//mode 0755
	mkdir(TINYLOGGER_SERVER_PID_DIR, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	
	std::fstream pidFile(TINYLOGGER_SERVER_PID_FILE, std::fstream::out);
	pidFile << getpid() << std::flush;
	pidFile.close();
}

void DeleteMasterPIDFile()
{
	unlink(TINYLOGGER_SERVER_PID_FILE);
	rmdir(TINYLOGGER_SERVER_PID_DIR);
}

int GetMasterPID()
{
	std::fstream pidFile(TINYLOGGER_SERVER_PID_FILE, std::fstream::in);
	if (pidFile.good() != true)
	{
		return -1;
	}
	std::string pid;
	getline(pidFile, pid);
	try
	{
		int p = std::stoi(pid);
		std::cout << p << std::endl;
		if (kill(p, 0) == 0)
		{
			return p;
		}
	}
	catch (...) {}
	return -1;
	
}

void MasterWork(int processAmount)
{
	while (true)
	{
		for (auto pid : workerPID)
		{
			if (kill(pid, 0) != 0)
			{
				workerPID.erase(pid);
			}
		}

		if (workerPID.size() < processAmount)
		{
			for (int i = workerPID.size(); i < processAmount; ++i)
			{
				int pid = fork();
				if (pid == 0)
				{
					SetProcessTitle("worker process deamon");
					server.StartHandleRequest();
					return;
				}
				else
				{
					workerPID.insert(pid);
				}
			}
		}
		
		sleep(5);
	}
}

void StartService()
{
	std::cout << "Initializing server..." << std::endl;
	SettingReader reader;
	reader.ReadConfiguration();
	
	int processAmount = 0;
	try
	{
		processAmount = std::stoi(reader.config["worker_process"]);
		if (processAmount <= 0)
		{
			throw std::runtime_error("...");
		}
	}
	catch (...) 
	{
		std::cout << "Invaild worker_process amount: " << reader.config["worker_process"] << std::endl;
		return;
	}

	try
	{
		std::cout << "Loaded facilities amount: " << server.AquireFacilities() << std::endl;
		server.StartListen();
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << std::endl;
		return;
	}

	for (int index = 0; index < processAmount; ++index)
	{
		int pid = fork();
		if (pid == 0)
		{
			worker = true;
			SetProcessTitle("worker process daemon");
			server.StartHandleRequest();
			return;
		}
		else
		{
			workerPID.insert(pid);
		}
	}
	std::cout << "Server now is running on " <<reader.config["address"] << ":" << reader.config["port"] << std::endl;
	
	daemon(0, 0);
	
	CreateMasterPIDFile();
	SetProcessTitle("master process daemon");
	MasterWork(processAmount);
}

void ShowStatus()
{
	if(GetMasterPID() != -1)
	{
		std::cout << "Server is running." << std::endl;
	}
	else
	{
		std::cout << "Server is not running." << std::endl;
	}
}

void StopService()
{
	std::cout << "Shutdown processing..." << std::endl;
	int pid = GetMasterPID();
	std::cout << pid << std::endl;
	if (pid != -1)
	{
		kill(pid, SIGTERM);
		while (true)
		{
			if (GetMasterPID() == -1)
			{
				break;
			}
			sleep(2);
		}
	}
	std::cout << "Server shutdown." << std::endl;
}

void CloseServer(int signal)
{
	if (worker == true)
	{
		server.GracefullyShutdown();
		return;
	}

	for (auto pid : workerPID)
	{
		kill(pid, SIGTERM);
	}
	DeleteMasterPIDFile();
	exit(0);
}

int main(int sys_argc, char **sys_argv, char **sys_environ)
{
	argv = sys_argv;
	environ = sys_environ;

	SetProcessTitleInit();
	signal(SIGTERM, CloseServer);

	if (getuid() != 0)
	{
		std::cout << "Need to run as super user" << std::endl;
		return 0;
	}

	int option = ArgumentParser(argv);
	switch (option)
	{
		case 1:
			StartService();
			break;
		case 2:
			ShowStatus();
			break;
		case 3:
			StopService();
			break;
		default:
			std::cout << "usage: tinyLogger {start|status|stop}" << std::endl;
			break;
	}
	
	return 0;
}
