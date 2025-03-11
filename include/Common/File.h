#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include "Common/Exception.h"

namespace Common {
namespace File {

	inline auto readRaw(
		std::filesystem::path const & filepath,
		auto createStorage,
		auto getStoragePtr,
		auto getResult // = [](auto & buf) -> std::decay_t<decltype(buf)> { return buf; } // when using defaults I get an error ... couldn't infer ...
	) {
		std::ifstream f(filepath);
		if (!f.good()) throw Common::Exception() << "failed to open file " << filepath;

		f.seekg(0, f.end);
		size_t size = f.tellg();
		f.seekg(0, f.beg);

		auto storage = createStorage(size);
		f.read(getStoragePtr(storage), size);
		return getResult(storage);
	}

	inline std::string read(std::filesystem::path const & filepath) {
		return readRaw(
			filepath,
			[](size_t size) { return std::vector<char>(size); },
			[](auto & buf) { return buf.data(); },
			[](auto & buf) { return std::string(buf.begin(), buf.end()); }
		);
	}

	template<typename T>
	inline std::vector<T> readAsVector(std::filesystem::path const & filepath) {
		return readRaw(
			filepath,
			[](size_t size) {
				if (size % sizeof(T) != 0) throw Common::Exception() << "this file size " << size << " does not align with the size of the struct you are reading: " << sizeof(T);
				return std::vector<T>(size / sizeof(T));
			},
			[](auto & buf) { return reinterpret_cast<char*>(buf.data()); },
			[](auto & buf) -> std::decay_t<decltype(buf)> { return buf; }
		);
	}

	void write(std::filesystem::path const & filepath, std::string const & data);
	bool exists(std::filesystem::path const & filepath);
	void remove(std::filesystem::path const & filepath);
}
}
