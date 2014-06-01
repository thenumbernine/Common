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

};

