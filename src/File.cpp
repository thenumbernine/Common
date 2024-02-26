#include "Common/File.h"
#include "Common/Exception.h"
#include <fstream>

namespace Common {
namespace File {

void write(std::filesystem::path const & filename, const std::string& data) {
	std::ofstream(filename) << data;
}

bool exists(std::filesystem::path const & filename) {
	return std::ifstream(filename).good();
}

void remove(std::filesystem::path const & filename) {
	std::remove(filename.c_str());
}

}
}
