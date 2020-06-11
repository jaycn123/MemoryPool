#include <iostream>
#include "memoryPool.h"

struct AStruct
{
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;
};

int main()
{
	MemoryManager::GetInstancePtr()->Init();
	
	using namespace std;
	clock_t time_start = clock();

	for (uint32_t i = 0; i < 10000000; i++)
	{
		auto pData = GETMEMORY(AStruct);
		MemoryManager::GetInstancePtr()->FreeMemory(sizeof(AStruct), (char*)pData);
	}
	clock_t time_end = clock();
	cout << "time use:" << 1000 * (time_end - time_start) / (double)CLOCKS_PER_SEC/1000 << "s" << endl;

	return 0;
}