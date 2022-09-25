#pragma once

//this is the poor mans version of Profiler library
#include <functional>
#include <string>

namespace Common {

void timeFunc(std::string const & name, std::function<void()> cb);

}
