#pragma once
#include <stdlib.h>
#include <functional>

template<typename RT>
class Function
{
public:
	template<class Func, typename... Args> Function(Func func, Args&&... args)
	{
		new(&action) std::function<RT()>([func, args...]()->RT{return (RT)(func(std::move(args)...)); });
	}
	template<class O, typename Func, typename... Args> Function(O& o, Func(O::*func), Args&&... args)
	{
		new(&action) std::function<RT()>([&o, func, args...]()->RT{return (RT)((o.*func)(std::move(args)...)); });
	}
	template<class O, typename Func, typename... Args> Function(O* o, Func(O::*func), Args&&... args)
	{
		new(&action) std::function<RT()>([o, func, args...]()->RT{return (RT)((o->*func)(std::move(args)...)); });
	}

	~Function() {}

	Function(const Function& func)
		:
		action(func.action)
	{}
	Function& operator=(const Function& func)
	{
		if (this != &func)
			action = func.action;

		return *this;
	}

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
