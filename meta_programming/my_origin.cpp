#pragma once
#include <iostream>

namespace MY
{

	 struct true_type { enum _value_ { value = true }; };
	  struct false_type { enum _value_ { value = false }; };

	   template <typename T>
		    struct is_int : false_type {};
	    template <>
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
		  template <typename T>
			   struct is_null_pointer : is_same<typename remove_const<T>::type, std::nullptr_t> {};

		   template <typename T>
			    struct is_integral_base : false_type {};
#define _IS_INTEGRAL_SPECIALIZATION_(x) template<> struct is_integral_base<x> : true_type {}
		    _IS_INTEGRAL_SPECIALIZATION_(signed short);
		     _IS_INTEGRAL_SPECIALIZATION_(signed int);
		      _IS_INTEGRAL_SPECIALIZATION_(signed long);
		       _IS_INTEGRAL_SPECIALIZATION_(signed long long);
		        _IS_INTEGRAL_SPECIALIZATION_(unsigned char);
			 _IS_INTEGRAL_SPECIALIZATION_(unsigned short);
			  _IS_INTEGRAL_SPECIALIZATION_(unsigned int);
			   _IS_INTEGRAL_SPECIALIZATION_(unsigned long);
			    _IS_INTEGRAL_SPECIALIZATION_(unsigned long long);
			     _IS_INTEGRAL_SPECIALIZATION_(int8_t);
			      template <typename T>
				       struct is_integral : is_integral_base<typename remove_const<T>::type> {};

			       template <typename T>
				        struct is_floating_point_base : false_type {};
#define _IS_FLOAT_POINT_SPECIALIZATION_(x) template<> struct is_floating_point_base<x> : true_type {}
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
					       struct is_fundamental : value_type<is_arithmetic<T>::value || is_null_pointer<T>::value || is_void<T>::value> {};
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
					     template <typename T>
						      struct is_reference_base<T&&> : true_type {};
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
							    struct is_signed_base : value_type < T(-1) < T(0) > {};
						    template <typename T>
							     struct is_signed_base<T, false> : false_type {};
						     template <typename T>
							      struct is_signed : is_signed_base<T> {};

						      template <typename T, bool _is_arithmetic = is_arithmetic<T>::value>
							       struct is_unsigned_base : value_type< T(0) < T(-1) > {};
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
#include <iostream>
using namespace std;

int main(void)
{
	 cout << "Meta Template Programming \n===================== (is_int<T>)" << endl;
	  cout << "is_int<bool>  : " << (MY::is_int<bool>::value ? "true": "false") << endl;
	   cout << "is_int<float> : " << (MY::is_int<float>::value? "true" : "false") << endl;
	    cout << "is_int<int>   : " << (MY::is_int<int>::value? "true" : "false") << endl;

	     cout << "===================== (is_same<T1, T2>)      ==================" << endl;
	      cout << "is_same<int, float> : " << MY::is_same<int, float>::value << endl;
	       cout << "is_same<int, long>  : " << MY::is_same<int, long>::value << endl;
	        cout << "is_same<int, int>   : " << MY::is_same<int, int>::value << endl;

		 cout << "===================== (is_void<T>)           ==================" << endl;
		  cout << "is_void<int>        : " << MY::is_void<int>::value << endl;
		   cout << "is_void<void>       : " << MY::is_void<void>::value << endl;
		    cout << "is_void<const void> : " << MY::is_void<const void>::value << endl;

		     cout << "===================== (is_null_pointer<T>)   ==================" << endl;
		      cout << "is_null_pointer<int*>              : " << MY::is_null_pointer<int*>::value << endl;
		      // cout << "is_null_pointer<NULL>: " << MY::is_null_pointer<NULL>::value << endl; // error
		       cout << "is_null_pointer<nullptr_t>         : " << MY::is_null_pointer<nullptr_t>::value << endl;
		        cout << "is_null_pointer<decltype(nullptr)> : " << MY::is_null_pointer<decltype(nullptr)>::value << endl;
			 cout << "is_null_pointer<const nullptr_t>   : " << MY::is_null_pointer<const nullptr_t>::value << endl;

			  cout << "===================== (is_integral<T>)       ==================" << endl;
			   cout << "is_integral<int>                : " << MY::is_integral<int>::value << endl;
			    cout << "is_integral<const unsigned long>: " << MY::is_integral<nullptr_t>::value << endl;
			     cout << "is_integral<decltype(nullptr)>  : " << MY::is_integral<decltype(nullptr)>::value << endl;
			      cout << "is_integral<float*>             : " << MY::is_null_pointer<float*>::value << endl;

			       cout << "===================== (is_floating_point<T>) ==================" << endl;
			        cout << "is_floating_point<double>             : " << MY::is_floating_point<double>::value << endl;
				 cout << "is_floating_point<const double>       : " << MY::is_floating_point<const double>::value << endl;
				  cout << "is_floating_point<decltype(nullptr)>  : " << MY::is_floating_point<decltype(nullptr)>::value << endl;
				   cout << "is_floating_point<float>              : " << MY::is_floating_point<const float>::value << endl;

				    class A {};
				     cout << "===================== (is_arithmetic<T>)     ==================" << endl;
				      cout << "is_arithmetic<Class A>                : " << MY::is_arithmetic<A>::value << endl;
				       cout << "is_arithmetic<int>                    : " << MY::is_arithmetic<int>::value << endl;
				        cout << "is_arithmetic<int const>              : " << MY::is_arithmetic<const int>::value << endl;
					 cout << "is_arithmetic<int &>                  : " << MY::is_arithmetic<int &>::value << endl;
					  cout << "is_arithmetic<int *>                  : " << MY::is_arithmetic<int *>::value << endl;
					   cout << "is_arithmetic<float>                  : " << MY::is_arithmetic<float>::value << endl;
					    cout << "is_arithmetic<float const>            : " << MY::is_arithmetic<const float>::value << endl;
					     cout << "is_arithmetic<float &>                : " << MY::is_arithmetic<float &>::value << endl;
					      cout << "is_arithmetic<float *>                : " << MY::is_arithmetic<float *>::value << endl;

					       cout << "===================== (is_fundamental<T>)    ==================" << endl;
					        cout << "is_fundamenal<Class A>                : " << MY::is_fundamental<A>::value << endl;
						 cout << "is_fundamenal<int>                    : " << MY::is_fundamental<int>::value << endl;
						  cout << "is_fundamenal<int &>                  : " << MY::is_fundamental<int &>::value << endl;
						   cout << "is_fundamenal<int *>                  : " << MY::is_fundamental<int *>::value << endl;
						    cout << "is_fundamenal<float>                  : " << MY::is_fundamental<float>::value << endl;
						     cout << "is_fundamenal<float &>                : " << MY::is_fundamental<float &>::value << endl;
						      cout << "is_fundamenal<float *>                : " << MY::is_fundamental<float *>::value << endl;
						       cout << "is_fundamenal<void>               l   : " << MY::is_fundamental<void>::value << endl;
						        cout << "is_fundamenal<nullptr_(t)>            : " << MY::is_fundamental<nullptr_t>::value << endl;

							 cout << "===================== (is_compound<T>)       ==================" << endl;
							  cout << "is_compound<Class A>                : " << MY::is_compound<A>::value << endl;
							   cout << "is_compound<int>                    : " << MY::is_compound<int>::value << endl;

							    cout << "===================== (is_pointer<T>)        ==================" << endl;
							     cout << "is_pointer<float &>                : " << MY::is_pointer<float &>::value << endl;
							      cout << "is_pointer<float *>                : " << MY::is_pointer<float *>::value << endl;

							       cout << "===================== (is_signed<T>)         ==================" << endl;
							        cout << "is_signed<int>                     : " << MY::is_signed<int>::value << endl;
								 cout << "is_signed<unsigned int>            : " << MY::is_signed<unsigned int>::value << endl;

								  cout << "===================== (is_unsigned<T>)       ==================" << endl;
								   cout << "is_unsigned<int>                   : " << MY::is_unsigned<int>::value << endl;
								    cout << "is_unsigned<unsigned int>          : " << MY::is_unsigned<unsigned int>::value << endl;

								     cout << "===================== (is_member_pointer<T>) ==================" << endl;
								      cout << "is_member_pointer<int(A::*)>        : " << MY::is_member_pointer<int(A::*)>::value << endl;
								       cout << "is_member_pointer<int>              : " << MY::is_member_pointer<int>::value << endl;

								        cout << "===================== (is_array<T>)          ==================" << endl;
									 cout << "is_array<A>                : " << MY::is_array<A>::value << endl;
									  cout << "is_array<A[]>              : " << MY::is_array<A[]>::value << endl;
}
