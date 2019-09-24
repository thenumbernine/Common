#pragma once

#include <functional>

namespace Common {

class Finally {
public:
	using Callback = std::function<void()>;
private:
	Callback f;
public:
	Finally(Callback f_) : f(f_) {}
	~Finally() noexcept(false) { f(); }
};

}
