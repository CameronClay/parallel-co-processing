#pragma once
#include <stdlib.h>
#include <functional>

class Function
{
public:
	template<class Func, typename... Args> Function(Func func, Args&&... args)
	{
		new(&action) std::function<void()>([func, &args...]()->RT{return (RT)(func(std::forward<Args>(args)...)); });
	}
	template<class O, typename Func, typename... Args> Function(O& o, Func(O::*func), Args&&... args)
	{
		new(&action) std::function<void()>([&o, func, &args...]()->RT{return (RT)((o.*func)(std::forward<Args>(args)...)); });
	}
	template<class O, typename Func, typename... Args> Function(O* o, Func(O::*func), Args&&... args)
	{
		new(&action) std::function<void()>([&o, func, &args...]()->RT{return (RT)((o->*func)(std::forward<Args>(args)...)); });
	}

	~Function() {}

	Function(Function&& func)
		:
		action(std::move(func.action))
	{}
	Function& operator=(Function&& func)
	{
		if (this != &func)
			action = std::move(func.action);

		return *this;
	}

	inline bool valid() const
	{
		return action.operator bool();
	}

	inline RT operator()()
	{
		return action();
	}
private:
	std::function<RT()> action;
};
