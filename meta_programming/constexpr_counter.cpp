#include  <iostream>

template<int N>
struct flag {
	  friend constexpr int adl_flag (flag<N>);
};

template<int N>
struct writer {
	  friend constexpr int adl_flag (flag<N>) {
		      return N;
		        }

	    static constexpr int value = N;
};

template<int N, int = adl_flag (flag<N> {})>
int constexpr reader (int, flag<N>) {
	  return N;
}

template<int N>
int constexpr reader (float, flag<N>, int R = reader (0, flag<N-1> {})) {
	  return R;
}

int constexpr reader (float, flag<0>) {
	  return 0;
}

template<int N = 1>
int constexpr next (int R = writer<reader (0, flag<32> {}) + N>::value) {
	  return R;
}

class Foo {

	  public:
		      static const int  Type = next();
};

class Foo2 {

	  public:
		      static const int  Type = next();
};

int main() {

	  std::cout << "Foo1 " << Foo::Type << std::endl;
	    std::cout << "Foo2 " << Foo2::Type << std::endl;
	      return 0;
}
