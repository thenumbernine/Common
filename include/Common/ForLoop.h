#pragma once

// TODO replace this with seq's template-based for-loop
//  and better yet replace that with a sequence-based lambda-based for-loop

namespace Common {

//for loop - compile-time indexes and execution of runtime code

template<int index, int end, template<int> class Exec, int step = (end > index ? 1 : -1)>
struct ForLoop {
	template<typename... InputArgs>
	static bool exec(InputArgs&&... input) {
		if (Exec<index>::exec(std::forward<InputArgs>(input)...)) return true;
		return ForLoop<index+step,end,Exec,step>::exec(std::forward<InputArgs>(input)...);
	}
};

template<int end, template<int> class Exec, int step>
struct ForLoop<end, end, Exec, step> {
	template<typename... InputArgs>
	static bool exec(InputArgs&&... input) {
		return false;
	}
};

}
