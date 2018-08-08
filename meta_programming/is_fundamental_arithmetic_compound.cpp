#include <iostream>

struct true_type { enum _value_ { value = true }; };

struct false_type { enum _value_ { value = false }; };

template <typename T>
struct remove_const { typedef T type; };
template <typename T>
struct remove_const<const T> { typedef T type; };

template <typename T>
struct is_integral_base : false_type {};

#define _IS_INTEGRAL_SPECIALIZATION_(x) template<>  struct is_integral_base<x> : true_type { }

_IS_INTEGRAL_SPECIALIZATION_(signed int);
_IS_INTEGRAL_SPECIALIZATION_(signed short);
_IS_INTEGRAL_SPECIALIZATION_(signed long);
_IS_INTEGRAL_SPECIALIZATION_(signed long long);
_IS_INTEGRAL_SPECIALIZATION_(unsigned int);
_IS_INTEGRAL_SPECIALIZATION_(unsigned short);
_IS_INTEGRAL_SPECIALIZATION_(unsigned long);
_IS_INTEGRAL_SPECIALIZATION_(unsigned long long);
_IS_INTEGRAL_SPECIALIZATION_(unsigned char);
//_IS_INTEGRAL_SPECIALIZATION_(char16_t);
//_IS_INTEGRAL_SPECIALIZATION_(char32_t);
//_IS_INTEGRAL_SPECIALIZATION_(wchar_t);

template <typename T>
struct is_integral : is_integral_base<typename remove_const<T>::type> {};

template <bool _value = false>
struct value_type : false_type{};
template<>
struct value_type<true> : true_type{};
template <typename T>
struct is_arithmetic : value_type<is_integral<T>::value || is_floating_point<T>

int main(void)
{
	using std::cout;
	using std::endl;

	cout << is_integral<int>::value << endl;
	cout << is_integral<const unsigned long long>::value << endl;
	cout << is_integral<int*>::value << endl;
	cout << is_integral<float>::value << endl;
	return 0;
}
