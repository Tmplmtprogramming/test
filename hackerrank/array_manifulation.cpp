#include <bits/stdc++.h>

#include <vector>
#include <algorithm>

using namespace std;

struct adding
{
	        int operator()(int k)
			        {
					                return data + k;
							        }
		        int data;
			        adding(int a) : data(a) {}
};

inline int operation(vector<int>& array, int a, int b, int k)
{
	        vector<int>::iterator itr = array.begin();
		        transform(itr+(a-1), itr+(b-1)+1, itr+(a-1), std::bind2nd(std::plus<int>(), k));
			        return 0;
}

int main()
{
	        int n;
		        int m;
			        int result = 0;

				        cin >> n >> m;

					        if(!(3 <= n && n <= 10^7))
							        {
									        return 0;
										        }
						        if(!(1 <= m && m <= 2*(10^5)))
								        {
										        return 0;
											        }

							        vector<int> array_vector(n, 0);

								        for(int a0 = 0; a0 < m; a0++)
										        {
												                int a;
														                int b;
																                int k;
																		                cin >> a >> b >> k;
																				                if(1 <= a && a <= b && b <= n)
																							                {
																										                    result = operation(array_vector, a, b, k);
																												                    }
																						        }
									        result = *max_element(array_vector.begin(), array_vector.end());
										        cout << result << endl;
											        return 0;
}
