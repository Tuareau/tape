#pragma once

#include <memory>

class function_wrapper
{
private:

	class callable_base 
	{
	public:
		virtual void call() = 0;
		virtual ~callable_base() = default;
	};

	std::unique_ptr<callable_base> callable_object;

	template <typename FucntionType>
	class callable : public callable_base 
	{
	private:
		FucntionType function;
	public:
		explicit callable(FucntionType && function) : function(std::move(function)) {}
		void call() override { this->function(); }
	};

public:
	template <typename FunctionType>
	explicit function_wrapper(FunctionType && function) {
		this->callable_object = std::make_unique<callable<FunctionType>>(std::move(function));
	}

	template <typename FucntionType>
	function_wrapper() {
		this->callable_object = std::make_unique<callable<FucntionType>>();
	}

	void operator()() {
		this->callable_object->call();
	}

	function_wrapper & operator=(function_wrapper && other) noexcept {
		this->callable_object = std::move(other.callable_object);
		return *this;
	}

	function_wrapper(const function_wrapper &) = delete;
	function_wrapper(function_wrapper &) = delete;
	function_wrapper & operator=(const function_wrapper &) = delete;
};

