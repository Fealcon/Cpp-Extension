#include "logger.hpp"

const char *  Log::logfile;
std::ofstream Log::fileStream;

bool Log::trace	   = true;
bool Log::warnings = true;
bool Log::errors   = true;

void Log::move_logs() {
	std::ifstream in(logfile, std::ifstream::ate | std::ifstream::binary);
	if (in.tellg() > 2E6) {
		in.close();

		std::string filename = logfile;
		remove((filename + "10.tar.gz").c_str());
		rename((filename + "9.tar.gz").c_str(), (filename + "10.tar.gz").c_str());
		rename((filename + "8.tar.gz").c_str(), (filename + "9.tar.gz").c_str());
		rename((filename + "7.tar.gz").c_str(), (filename + "8.tar.gz").c_str());
		rename((filename + "6.tar.gz").c_str(), (filename + "7.tar.gz").c_str());
		rename((filename + "5.tar.gz").c_str(), (filename + "6.tar.gz").c_str());
		rename((filename + "4.tar.gz").c_str(), (filename + "5.tar.gz").c_str());
		rename((filename + "3.tar.gz").c_str(), (filename + "4.tar.gz").c_str());
		system(("tar -zcf " + filename + "3.tar.gz -C / " + filename.substr(1) + "2").c_str());
		rename((filename + "1").c_str(), (filename + "2").c_str());
		rename(filename.c_str(), (filename + "1").c_str());
	}
}
