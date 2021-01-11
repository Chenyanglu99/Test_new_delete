#include "my_allocator.h"
#include <vector>


int main(void)
{
	int ia[5] = { 1, 2, 3, 4 };
	unsigned int i;

	std::vector<int, test_my_namespace::my_allocator<int>> iv(ia, ia + 5);
	
	for(i = 0;i < iv.size(); i++)
	{

		std::cout << iv[i] << ' ';
	}

	std::cout << std::endl;
	
	getchar();
	return 0;
}