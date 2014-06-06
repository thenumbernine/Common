#include "Common/File.h"

namespace Common {

std::string File::read(std::string filename) {
	std::ifstream f(filename);
	f.seekg(0, f.end);
	size_t len = f.tellg();
	f.seekg(0, f.beg);
	std::vector<char> buf(len);
	f.read(&buf[0], len);
	return std::string(buf.begin(), buf.end());
}

void File::write(std::string filename, std::string data) {
	std::ofstream(filename) << data;
}

std::string File::getExtension(std::string filename) {
	std::string::size_type i = filename.rfind('.');
	if (i == std::string::npos) return filename;
	return filename.substr(i+1);
}

bool File::exists(std::string filename) {
	return std::ifstream(filename).good();
}

void File::remove(std::string filename) {
	std::remove(filename.c_str());
}

};

