#include <iostream>

using namespace std;

struct true_type { enum _value_ { value = true }; };
struct false_type { enum _value_ { value = false }; };

template <typename T1, typename T2>
struct is_same : false_type { };
template <typename T>
struct is_same<T, T> : true_type { };

template <typename T>
struct remove_const { typedef T type; };
template <typename T>
struct remove_const<const T> { typedef T type; };

template <typename T>
struct is_void : is_same<typename remove_const<T>::type, void> { };

template <typename T>
struct is_integral_base : false_type { };
#define _IS_INTEGRAL_SPECIALIZATION_(X) template <> struct is_integral_base<X> : true_type { }
_IS_INTEGRAL_SPECIALIZATION_(signed char);
_IS_INTEGRAL_SPECIALIZATION_(signed int);
_IS_INTEGRAL_SPECIALIZATION_(signed short);
_IS_INTEGRAL_SPECIALIZATION_(signed long);
_IS_INTEGRAL_SPECIALIZATION_(signed long long);
_IS_INTEGRAL_SPECIALIZATION_(unsigned char);
_IS_INTEGRAL_SPECIALIZATION_(unsigned int);
_IS_INTEGRAL_SPECIALIZATION_(unsigned short);
_IS_INTEGRAL_SPECIALIZATION_(unsigned long);
_IS_INTEGRAL_SPECIALIZATION_(unsigned long long);
template <typename T>
struct is_integral : is_integral_base<typename remove_const<T>::type> { };

template <typename T>
struct is_floating_point_base : false_type { };
#define _IS_FLOAT_POINT_SPECIALIZATION_(X) template<> struct is_floating_point_base<X> : true_type { }
_IS_FLOAT_POINT_SPECIALIZATION_(float);
_IS_FLOAT_POINT_SPECIALIZATION_(double);
_IS_FLOAT_POINT_SPECIALIZATION_(long double);
template <typename T>
struct is_floating_point : is_floating_point_base<typename remove_const<T>::type> { };

template <bool _value = false>
struct value_type : false_type { };
template <>
struct value_type<true> : true_type { };

template <typename T>
struct is_arithmetic : value_type<is_integral<T>::value || is_floating_point<T>::value> { };
template <typename T>
struct is_fundamental : value_type<is_arithmetic<T>::value || /*is_null_pointer<T>::value ||*/ is_void<T>::value> { };
template <typename T>
struct is_compound : value_type<!is_fundamental<T>::value> { };

template <typename T>
struct is_pointer_base : false_type { };
template <typename T>
struct is_pointer_base<T*> : true_type { };
template <typename T>
struct is_pointer : is_pointer_base<typename remove_const<T>::type> { };

template <typename T>
struct is_reference_base : false_type {};
template <typename T>
struct is_reference_base<T&> : true_type {};
//template <typename T>
//struct is_reference_base<T&&> : true_type {};
template <typename T>
struct is_reference : is_reference<typename remove_const<T>::type> {};

template <typename T>
struct is_const_base : false_type {};
template <typename T>
struct is_const_base<const T> : true_type {};
template <typename T>
struct is_const : is_const_base<T> {};

template <typename T>
struct is_volatile_base : false_type {};
template <typename T>
struct is_volatile_base<volatile T> : true_type {};
template <typename T>
struct is_volatile : is_volatile_base<T> {};

template <typename T, bool _is_arithmetic = is_arithmetic<T>::value>
struct is_signed_base : value_type<T(-1) < T(0)> {};
template <typename T>
struct is_signed_base<T, false> : false_type {};
template <typename T>
struct is_signed : is_signed_base<T> {};

template <typename T, bool _is_arithmetic = is_arithmetic<T>::value>
struct is_unsigned_base : value_type<T(0) < T(-1)> {};
template <typename T>
struct is_unsigned_base<T, false> : false_type {};
template <typename T>
struct is_unsigned : is_unsigned_base<T> {};

class A {};

class cls {};

//enum B : int {};
//enum class C : int {};

int main(void)
{
	cout << "is_same" << endl;
	cout << is_same <int, float> :: value << endl;
	cout << is_same <int, long> :: value << endl;
	cout << is_same <int, int> :: value << endl;

	cout << "is_void" << endl;
	cout << is_void<int>::value << endl;
	cout << is_void<void>::value << endl;
	cout << is_void<const void>::value << endl;

	cout << "is_integral" << endl;
	cout << is_integral<int>::value << endl;
	cout << is_integral<const unsigned long long>::value << endl;
	cout << is_integral<int*>::value << endl;
	cout << is_integral<float>::value << endl;

	cout << "is_floating_point" << endl;
	cout << is_floating_point<int>::value << endl;
	cout << is_floating_point<const unsigned long long>::value << endl;
	cout << is_floating_point<int*>::value << endl;
	cout << is_floating_point<float>::value << endl;


	cout << "is_arithmetic" << endl;
	cout << "A:           " << is_arithmetic<A>:: value << endl;
	cout << "int:         " << is_arithmetic<int>:: value << endl;
	cout << "int const:   " << is_arithmetic<int const>:: value << endl;
	cout << "int &:       " << is_arithmetic<int&>:: value << endl;
	cout << "int *:       " << is_arithmetic<int*>:: value << endl;
	cout << "float:       " << is_arithmetic<float>:: value << endl;
	cout << "float const: " << is_arithmetic<float const>:: value << endl;
	cout << "float &:     " << is_arithmetic<float&>:: value << endl;
	cout << "float *:     " << is_arithmetic<float*>:: value << endl;

	cout << "is_fundamental" << endl;
	cout << "A:           " << is_fundamental<A>:: value << endl;
	cout << "int:         " << is_fundamental<int>:: value << endl;
	cout << "int const:   " << is_fundamental<int const>:: value << endl;
	cout << "int &:       " << is_fundamental<int&>:: value << endl;
	cout << "int *:       " << is_fundamental<int*>:: value << endl;
	cout << "float:       " << is_fundamental<float>:: value << endl;
	cout << "float const: " << is_fundamental<float const>:: value << endl;
	cout << "float &:     " << is_fundamental<float&>:: value << endl;
	cout << "float *:     " << is_fundamental<float*>:: value << endl;

	cout << "is_compound" << endl;
	cout << (is_compound<cls>::value ? "cls is compound" : "cls is not a compound") << endl;
	cout << (is_compound<int>::value ? "int is compound" : "int is not a compound") << endl;

	cout << "is_pointer" << endl;
	cout << is_pointer<A>::value << endl;
	cout << is_pointer<A*>::value << endl;
	cout << is_pointer<A &>::value << endl;
	cout << is_pointer<int>::value << endl;
	cout << is_pointer<int *>::value << endl;
	cout << is_pointer<int **>::value << endl;
	cout << is_pointer<int[10]>::value << endl;
	//cout << is_pointer<nullptr_t>::value << endl;
	cout << endl;

	cout << "is_reference" << endl;
	cout << is_pointer<A>::value << endl;
	cout << is_pointer<A&>::value << endl;
//	cout << is_pointer<A&&>::value << endl;
	cout << is_pointer<int>::value << endl;
	cout << is_pointer<int&>::value << endl;
//	cout << is_pointer<int&&>::value << endl;
	cout << endl;

	cout << "is_const" << endl;
	cout << is_const<int>::value << endl;
	cout << is_const<const int>::value << endl;
	cout << endl;

	cout << "is_volatile" << endl;
	cout << is_volatile<int>::value << endl;
	cout << is_volatile<volatile int>::value << endl;
	cout << endl;

	cout << "is_signed" << endl;
	cout << is_signed<A>::value << endl;
	cout << is_signed<float>::value << endl;
	cout << is_signed<signed int>::value << endl;
	cout << is_signed<unsigned int>::value << endl;
	//cout << is_signed<B>::value << endl;
	//cout << is_signed<C>::value << endl;
	cout << endl;

	cout << "is_unsigned" << endl;
	cout << is_unsigned<A>::value << endl;
	//cout << is_unsigned<float>::value << endl;
	cout << is_unsigned<signed int>::value << endl;
	cout << is_unsigned<unsigned int>::value << endl;
	//cout << is_unsigned<B>::value << endl;
	//cout << is_unsigned<C>::value << endl;
	cout << endl;
	return 0;
}
