#include <iostream>

struct true_type
{
	enum _value_
	{
		value = true
	};
};

struct false_type
{
	enum _value_
	{
		value = false
	};
};

template <typename T1, typename T2>
//struct is_same<T1, T2> : false_type
struct is_same : false_type
{
};

template <typename T>
struct is_same<T, T> : true_type
{
};

template<typename T>
struct remove_const
{
	typedef T type;
};

template<typename T>
struct remove_const<const T>
{
	typedef T type;
};

template <typename T>
struct is_void : is_same<typename remove_const<T>::type, void>
{
};

//template <typename T>
//struct is_void : is_same<T, void>
//{
//};

//template <typename T>
//struct is_null_pointer : is_same<T, NULL>
//{
//};

int main()
{
	using std::cout;
	using std::endl;

	cout << "is_void" << endl;
	cout << is_void<int>::value << endl;
	cout << is_void<void>::value << endl;
	cout << is_void<const void>::value << endl;

	cout << "is_nullptr" << endl;
//	cout << is_null_pointer<int*>::value << endl;
//	cout << is_null_pointer<NULL>::value << endl;
//	cout << is_null_pointer<decltype(nullptr)>::value << endl;
//	cout << is_null_pointer<const std::nullptr_t>::value << endl;
}
