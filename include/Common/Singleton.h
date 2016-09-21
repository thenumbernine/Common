#pragma once

namespace Common {

template<typename T>
class Singleton {
protected:
	const char *initd;
	T *t;
	
	void validate() {
		static const char *verification = "verification";
		if (initd != verification) {
			initd = verification;
			t = new T();
		}
	}
public:
	Singleton() {
		validate();
	}
	virtual ~Singleton() {
		delete t;	
		initd = 0;
		t = 0;
	}
	operator T*() {
		validate();
		return t;
	}
	T *operator->() {
		validate();
		return t;
	}
};

};

