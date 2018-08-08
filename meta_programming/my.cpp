#include <stdint.h>
#include <stdlib.h>

namespace Zeppelin
{
#if defined (_MSC_VER)
#define ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#define  ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ALWAYS_INLINE inline
#endif
	/**
	  * Anonymous class, to be used instead of
	  * 0 or NULL.
	  * Enables the selection of the correct form
	  * when methods are overloaded for
	  * both pointers & integrals types.
	  */
	const class
	{
		public:
#if defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ == 5) && (__GNUC_PATHLEVEL < 2)
			ALWAYS_INLINE operator void*() const
			{
				return 0;
			}
#endif
			template <typename T>
			ALWAYS_INLINE operator T*() const
			{
				return 0;
			}

			template <class C, typename T>
			ALWAYS_INLINE operator T C::*() const
			{
				return 0;
			}
		private:
			void operator &() const;
	} nullptr = {};
	// The empty brace initializer fills the ISO C++
	// (in 8.5 [dcl.ini] paragraph 9) requirements :
	//
	// If no initializer is specified for an object, and the object is of (possibly cv-qualified) non-POD class
	// type (or array thereof), the object shall be
	// default-iniitalized; if the object is of
	// const-qualified type, the underlying class type shall
	// have a user-declared default constructor

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

	template <typename T>
	struct is_int : false_type {};
	template<>
	struct is_int<int> : true_type {};

	template <typename T1, typename T2>
	struct is_same : false_type {};
	template <typename T>
	struct is_same<T, T> : true_type {};

	template <typename T>
	struct remove_const { typedef T type; };
	template <typename T>
	struct remove_const<const T> { typedef T type; };

	template <typename T>
	struct is_void : is_same<typename remove_const<T>::type, void> {};
	//template <typename T>
	//struct is_null_pointer : is_same<typename remove_const<T>::type, std::nullptr_t> {};

	template <typename T>
	struct is_integral_base : false_type {};

#define _IS_INTEGRAL_SPECIALIZATION_(x) template<> struct is_integral_base<x> : true_type {}
	_IS_INTEGRAL_SPECIALIZATION_(signed char);
	_IS_INTEGRAL_SPECIALIZATION_(signed short);
	_IS_INTEGRAL_SPECIALIZATION_(signed int);
	_IS_INTEGRAL_SPECIALIZATION_(signed long);
	_IS_INTEGRAL_SPECIALIZATION_(signed long long);
	_IS_INTEGRAL_SPECIALIZATION_(unsigned char);
	_IS_INTEGRAL_SPECIALIZATION_(unsigned short);
	_IS_INTEGRAL_SPECIALIZATION_(unsigned int);
	_IS_INTEGRAL_SPECIALIZATION_(unsigned long);
	_IS_INTEGRAL_SPECIALIZATION_(unsigned long long);
	//_IS_INTEGRAL_SPECIALIZATOIN_(int8_t);
	template <typename T>
	struct is_integral : is_integral_base<typename remove_const<T>::type> {};

	template <typename T>
	struct is_floating_point_base : false_type{};
#define _IS_FLOAT_POINT_SPECIALIZATION_(X) template<> struct is_floating_point_base<X> : true_type {}
	_IS_FLOAT_POINT_SPECIALIZATION_(float);
	_IS_FLOAT_POINT_SPECIALIZATION_(double);
	template <typename T>
	struct is_floating_point : is_floating_point_base<typename remove_const<T>::type> {};

	template <bool _value_ = false>
	struct value_type : false_type {};
	template <>
	struct value_type<true> : true_type {};

	template <typename T>
	struct is_arithmetic : value_type<is_integral<T>::value || is_floating_point<T>::value> {};
	template <typename T>
	struct is_fundamental : value_type<is_arithmetic<T>::value || /*is_null_pointer<T>::value || */is_void<T>::value> {};
	template <typename T>
	struct is_compound : value_type<!is_fundamental<T>::value> {};

	template <typename T>
	struct is_pointer_base : false_type {};
	template <typename T>
	struct is_pointer_base<T*> : true_type {};
	template <typename T>
	struct is_pointer : is_pointer_base<typename remove_const<T>::type> {};

	template <typename T>
	struct is_reference_base : false_type {};
	template <typename T>
	struct is_reference_base<T&> : true_type {};
	//template <typename T>
	//struct is_reference_base<T&&> : true_type {};
	template <typename T>
	struct is_reference : is_reference_base<typename remove_const<T>::type> {};

	template <typename T>
	struct is_const : false_type {};
	template <typename T>
	struct is_const<const T> : true_type {};

	template <typename T>
	struct is_volatile : false_type {};
	template <typename T>
	struct is_volatile<volatile T> : true_type {};

	template <typename T, bool _is_arithmetic = is_arithmetic<T>::value>
	struct is_signed_base : value_type<T(-1) < T(0)> {};
	template <typename T>
	struct is_signed_base<T, false> : false_type {};
	template <typename T>
	struct is_signed : is_signed_base<T> {};

	template <typename T, bool _is_arithmetic = is_arithmetic<T>::value>
	struct is_unsigned_base : value_type < T(0) < T(-1) > {};
	template <typename T>
	struct is_unsigned_base<T, false> : false_type {};
	template <typename T>
	struct is_unsigned : is_unsigned_base<T> {};

	template <typename T>
	struct is_member_pointer_base : false_type {};
	template <typename T, typename U>
	struct is_member_pointer_base<T U::*> : true_type {};
	template <typename T>
	struct is_member_pointer : is_member_pointer_base<typename remove_const<T>::type> {};

	template <typename T>
	struct is_array : false_type {};
	template <typename T>
	struct is_array<T[]> : true_type {};
	template <typename T, std::size_t size>
	struct is_array<T[size]> : true_type {};
}

#if 1
#include <iostream>
using namespace std;

int main(void)
{
	cout << "Meta Template Programming \n===================== (is_int<T>)" << endl;
	cout << "is_int<bool>  : " << (Zeppelin::is_int<bool>::value ? "true": "false") << endl;
	cout << "is_int<float> : " << (Zeppelin::is_int<float>::value? "true" : "false") << endl;
	cout << "is_int<int>   : " << (Zeppelin::is_int<int>::value? "true" : "false") << endl;

	cout << "===================== (is_same<T1, T2>)      ==================" << endl;
	cout << "is_same<int, float> : " << Zeppelin::is_same<int, float>::value << endl;
	cout << "is_same<int, long>  : " << Zeppelin::is_same<int, long>::value << endl;
	cout << "is_same<int, int>   : " << Zeppelin::is_same<int, int>::value << endl;

	cout << "===================== (is_void<T>)           ==================" << endl;
	cout << "is_void<int>        : " << Zeppelin::is_void<int>::value << endl;
	cout << "is_void<void>       : " << Zeppelin::is_void<void>::value << endl;
	cout << "is_void<const void> : " << Zeppelin::is_void<const void>::value << endl;

	//cout << "===================== (is_null_pointer<T>)   ==================" << endl;
	//cout << "is_null_pointer<int*>              : " << Zeppelin::is_null_pointer<int*>::value << endl;
	//// cout << "is_null_pointer<NULL>: " << Zeppelin::is_null_pointer<NULL>::value << endl; // error
	//cout << "is_null_pointer<nullptr_t>         : " << Zeppelin::is_null_pointer<nullptr_t>::value << endl;
	//cout << "is_null_pointer<decltype(nullptr)> : " << Zeppelin::is_null_pointer<decltype(nullptr)>::value << endl;
	//cout << "is_null_pointer<const nullptr_t>   : " << Zeppelin::is_null_pointer<const nullptr_t>::value << endl;

	cout << "===================== (is_integral<T>)       ==================" << endl;
	cout << "is_integral<int>                : " << Zeppelin::is_integral<int>::value << endl;
	//cout << "is_integral<const unsigned long>: " << Zeppelin::is_integral<nullptr_t>::value << endl;
	//cout << "is_integral<decltype(nullptr)>  : " << Zeppelin::is_integral<decltype(nullptr)>::value << endl;
	//cout << "is_integral<float*>             : " << Zeppelin::is_null_pointer<float*>::value << endl;

	cout << "===================== (is_floating_point<T>) ==================" << endl;
	cout << "is_floating_point<double>             : " << Zeppelin::is_floating_point<double>::value << endl;
	cout << "is_floating_point<const double>       : " << Zeppelin::is_floating_point<const double>::value << endl;
	cout << "is_floating_point<decltype(nullptr)>  : " << Zeppelin::is_floating_point<decltype(nullptr)>::value << endl;
	cout << "is_floating_point<float>              : " << Zeppelin::is_floating_point<const float>::value << endl;

	class A {};
	cout << "===================== (is_arithmetic<T>)     ==================" << endl;
	cout << "is_arithmetic<Class A>                : " << Zeppelin::is_arithmetic<A>::value << endl;
	cout << "is_arithmetic<int>                    : " << Zeppelin::is_arithmetic<int>::value << endl;
	cout << "is_arithmetic<int const>              : " << Zeppelin::is_arithmetic<const int>::value << endl;
	cout << "is_arithmetic<int &>                  : " << Zeppelin::is_arithmetic<int &>::value << endl;
	cout << "is_arithmetic<int *>                  : " << Zeppelin::is_arithmetic<int *>::value << endl;
	cout << "is_arithmetic<float>                  : " << Zeppelin::is_arithmetic<float>::value << endl;
	cout << "is_arithmetic<float const>            : " << Zeppelin::is_arithmetic<const float>::value << endl;
	cout << "is_arithmetic<float &>                : " << Zeppelin::is_arithmetic<float &>::value << endl;
	cout << "is_arithmetic<float *>                : " << Zeppelin::is_arithmetic<float *>::value << endl;

	cout << "===================== (is_fundamental<T>)    ==================" << endl;
	cout << "is_fundamenal<Class A>                : " << Zeppelin::is_fundamental<A>::value << endl;
	cout << "is_fundamenal<int>                    : " << Zeppelin::is_fundamental<int>::value << endl;
	cout << "is_fundamenal<int &>                  : " << Zeppelin::is_fundamental<int &>::value << endl;
	cout << "is_fundamenal<int *>                  : " << Zeppelin::is_fundamental<int *>::value << endl;
	cout << "is_fundamenal<float>                  : " << Zeppelin::is_fundamental<float>::value << endl;
	cout << "is_fundamenal<float &>                : " << Zeppelin::is_fundamental<float &>::value << endl;
	cout << "is_fundamenal<float *>                : " << Zeppelin::is_fundamental<float *>::value << endl;
	cout << "is_fundamenal<void>               l   : " << Zeppelin::is_fundamental<void>::value << endl;
	cout << "is_fundamenal<nullptr_(t)>            : " << Zeppelin::is_fundamental<nullptr_t>::value << endl;

	cout << "===================== (is_compound<T>)       ==================" << endl;
	cout << "is_compound<Class A>                : " << Zeppelin::is_compound<A>::value << endl;
	cout << "is_compound<int>                    : " << Zeppelin::is_compound<int>::value << endl;

	cout << "===================== (is_pointer<T>)        ==================" << endl;
	cout << "is_pointer<float &>                : " << Zeppelin::is_pointer<float &>::value << endl;
	cout << "is_pointer<float *>                : " << Zeppelin::is_pointer<float *>::value << endl;

	cout << "===================== (is_signed<T>)         ==================" << endl;
	cout << "is_signed<int>                     : " << Zeppelin::is_signed<int>::value << endl;
	cout << "is_signed<unsigned int>            : " << Zeppelin::is_signed<unsigned int>::value << endl;

	cout << "===================== (is_unsigned<T>)       ==================" << endl;
	cout << "is_unsigned<int>                   : " << Zeppelin::is_unsigned<int>::value << endl;
	cout << "is_unsigned<unsigned int>          : " << Zeppelin::is_unsigned<unsigned int>::value << endl;

	cout << "===================== (is_member_pointer<T>) ==================" << endl;
	cout << "is_member_pointer<int(A::*)>        : " << Zeppelin::is_member_pointer<int(A::*)>::value << endl;
	cout << "is_member_pointer<int>              : " << Zeppelin::is_member_pointer<int>::value << endl;

	cout << "===================== (is_array<T>)          ==================" << endl;
	cout << "is_array<A>                : " << Zeppelin::is_array<A>::value << endl;
	cout << "is_array<A[]>              : " << Zeppelin::is_array<A[]>::value << endl;
}
#endif
