#include <string>
#include <fstream>
#include <vector>

namespace Common {

struct File {
	static std::string read(const std::string& filename);
	static void write(const std::string& filename, const std::string& data);
	static std::string getExtension(const std::string& filename);
	static bool exists(const std::string& filename);
	static void remove(const std::string& filename);
};

};

