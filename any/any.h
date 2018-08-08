//#pragma once

#include <exception>
#include <memory>
#include <typeinfo>
#include <type_traits>

struct bad_any_cast : public std::bad_cast { };
class any;
void swap(any &, any &);
template<typename Type> Type any_cast(any&);
template<typename Type> Type any_cast(any&&);
template<typename Type> Type any_cast(const any&);
template<typename Type> const Type* any_cast(const any*);
template<typename Type> Type* any_cast(any*);

//template<typename Type> const Type* unsafe_any_cast(const any* operand);
//template<typename Type> Type* unsafe_any_cast(any* operand);

class any
{
public:

	template<typename Type> friend 
	Type any_cast(any&);

	template<typename Type> 
	friend Type any_cast(const any&);

	template<typename Type> 
	friend Type* any_cast(any*);

	template<typename Type> 
	friend const Type* any_cast(const any*);

	any() : ptr(nullptr) {} 
	any(any&& x) : ptr(std::move(x.ptr)) {}

	any(const any& x)
	{
		if (x.ptr)
		{
			ptr = x.ptr->clone();
		}
	}

	template<typename Type>
	any(const Type& x) 
	: ptr(new concrete<typename std::decay<const Type>::type>(x)) 
	{}

	any& operator=(any&& rhs)
	{
		ptr = std::move(rhs.ptr);
		return (*this);
	}

	any& operator=(const any& rhs)
	{
		ptr = std::move(any(rhs).ptr);
		return (*this);
	}

	template<typename T>
	any& operator=(T&& x)
	{
		ptr.reset(new concrete<typename std::decay<T>::type>(typename std::decay<T>::type(x)));
		return (*this);
	}  

	template<typename T>
	any& operator=(const T& x)
	{
		ptr.reset(new concrete<typename std::decay<T>::type>(typename std::decay<T>::type(x)));
		return (*this);
	}

	void clear()
	{ 
		ptr.reset(nullptr); 
	}

	bool empty() const
	{ 
		return ptr == nullptr; 
	}

	const std::type_info& type() const
	{ 
		return (!empty()) ? ptr->type() : typeid(void); 
	}

private:

	struct placeholder
	{
		virtual std::unique_ptr<placeholder> clone() const = 0;
		virtual const std::type_info& type() const = 0;
		virtual ~placeholder() {}
	};

	template<typename T>
	struct concrete : public placeholder
	{
		//concrete(T&& x) : value(std::move(x)) 
		//{}

		concrete(const T& x) : value(x) {}

		virtual std::unique_ptr<placeholder> clone() const override
		{
			return std::unique_ptr<placeholder>(new concrete<T>(value));
		}

		virtual const std::type_info& type() const override
		{ 
			return typeid(T); 
		}

		T value;
	};

	std::unique_ptr<placeholder> ptr;

};

template<typename Type> 
Type any_cast(any& val)
{
	if (val.ptr->type() != typeid(Type))
	{
		throw bad_any_cast();
	}
	return static_cast<any::concrete<Type>*>(val.ptr.get())->value;
}

template<typename Type> 
Type any_cast(const any& val)
{
	return any_cast<Type>(any(val));
}

template<typename Type> 
Type* any_cast(any* rhs)
{
	return dynamic_cast<Type*>(rhs->ptr.get());
}

template<typename Type> 
const Type* any_cast(const any* rhs)
{
	return dynamic_cast<const Type*>(rhs->ptr.get());
}

template<typename Type>
const Type* unsafe_any_cast(const any* operand)
{
	return unsafe_any_cast<Type>(const_cast<any*>(operand));
}

template<typename Type>
Type* unsafe_any_cast(any* operand)
{
	return &static_cast<any::concrete<Type>*>(operand->ptr)->held;
	//return static_cast<Type>(operand->ptr.get());
}
