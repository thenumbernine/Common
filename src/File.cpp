#include "Common/File.h"
#include "Common/Exception.h"
#include <fstream>

namespace Common {
namespace File {

void write(const std::string& filename, const std::string& data) {
	std::ofstream(filename) << data;
}

std::string getExtension(const std::string& filename) {
	std::string::size_type i = filename.rfind('.');
	if (i == std::string::npos) return filename;
	return filename.substr(i+1);
}

bool exists(const std::string& filename) {
	return std::ifstream(filename).good();
}

void remove(const std::string& filename) {
	std::remove(filename.c_str());
}

}
}
