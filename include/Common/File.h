#pragma once

#include <string>
#include <fstream>
#include <vector>
#include "Exception.h"

namespace Common {
namespace File {
	
	//I would like return template specializatoin ...
	//template<typename Result = std::string>
	//static Result read(std::string const & filename);
	//inline std::string read<std::string>(std::string const & filename) {
	// but instead I'm stuck with ...
	inline std::string read(std::string const & filename) {
		std::ifstream f(filename);
		if (!f.good()) throw Common::Exception() << "failed to open file " << filename;
		f.seekg(0, f.end);
		size_t len = f.tellg();
		f.seekg(0, f.beg);
		
		std::vector<char> buf(len);
		
		f.read(&buf[0], len);
		
		return std::string(buf.begin(), buf.end());
	}

	template<typename T>
	inline std::vector<T> readAsVector(std::string const & filename) {
		std::ifstream f(filename);
		if (!f.good()) throw Common::Exception() << "failed to open file " << filename;
		f.seekg(0, f.end);
		size_t len = f.tellg();
		f.seekg(0, f.beg);
		
		std::vector<T> result(len);
		
		f.read(result.data(), len);
		
		return result;
	}
	
	void write(const std::string& filename, const std::string& data);
	std::string getExtension(const std::string& filename);
	bool exists(const std::string& filename);
	void remove(const std::string& filename);
}
}
