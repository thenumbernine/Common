#pragma once

#include <functional>

class Finally {
public:
	typedef std::function<void()> Callback;
private:
	Callback f;
public:
	Finally(Callback f_) : f(f_) {}
	~Finally() {
		f();
	}
};

