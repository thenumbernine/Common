#include "Common/Profile.h"
#include <iostream>
#include <chrono>

namespace Common {

void timeFunc(std::string const & name, std::function<void()> cb) {
	std::cout << name << "..." << std::endl;
	std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	cb();
	std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> dt = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
	std::cout << "...done " << name << " (" << dt.count() << "s)" << std::endl;
}

}
