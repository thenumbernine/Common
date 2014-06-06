#include <string>
#include <fstream>
#include <vector>

namespace Common {

struct File {
	static std::string read(std::string filename);
	static void write(std::string filename, std::string data);
	static std::string getExtension(std::string filename);
	static bool exists(std::string filename);
	static void remove(std::string filename);
};

};

