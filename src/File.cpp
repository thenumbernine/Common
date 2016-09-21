#include "Common/File.h"

namespace Common {

std::string File::read(const std::string& filename) {
	std::ifstream f(filename);
	f.seekg(0, f.end);
	size_t len = f.tellg();
	f.seekg(0, f.beg);
	std::vector<char> buf(len);
	f.read(&buf[0], len);
	return std::string(buf.begin(), buf.end());
}

void File::write(const std::string& filename, const std::string& data) {
	std::ofstream(filename) << data;
}

std::string File::getExtension(const std::string& filename) {
	std::string::size_type i = filename.rfind('.');
	if (i == std::string::npos) return filename;
	return filename.substr(i+1);
}

bool File::exists(const std::string& filename) {
	return std::ifstream(filename).good();
}

void File::remove(const std::string& filename) {
	std::remove(filename.c_str());
}

};

