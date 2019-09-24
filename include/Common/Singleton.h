#pragma once

namespace Common {

// https://codereview.stackexchange.com/questions/173929/modern-c-singleton-template

template<typename T>
class Singleton {
protected:
	static T* instance() {
		static T t;
		return &t;
	}
public:
	operator T*() {
		return instance();
	}
	T *operator->() {
		return instance();
	}
};

}
