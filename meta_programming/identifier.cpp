#include <iostream>

using namespace std;

class CFooComponent
{
	private:
		static _component_identifier component_identifier;

	public:
		static size_t get_componentId(void)
		{
			return reinterpret_cast<size_t>(&CFooComponent::
		}
};
