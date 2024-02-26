#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include "Common/Exception.h"

namespace Common {
namespace File {
	
	//I would like return template specializatoin ...
	//template<typename Result = std::string>
	//static Result read(std::string const & filename);
	//inline std::string read<std::string>(std::string const & filename) {
	// but instead I'm stuck with ...
	inline std::string read(std::filesystem::path const & filename) {
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
	inline std::vector<T> readAsVector(std::filesystem::path const & filename) {
		std::ifstream f(filename);
		if (!f.good()) throw Common::Exception() << "failed to open file " << filename;
		f.seekg(0, f.end);
		size_t len = f.tellg();
		if (len % sizeof(T) != 0) throw Common::Exception() << "this file size " << len << " does not align with the size of the struct you are reading: " << sizeof(T);
		f.seekg(0, f.beg);
		
		std::vector<T> result(len / sizeof(T));
		
		f.read((char*)result.data(), len);
		
		return result;
	}
	
	void write(std::filesystem::path const & filename, std::string const & data);
	bool exists(std::filesystem::path const & filename);
	void remove(std::filesystem::path const & filename);
}
}
