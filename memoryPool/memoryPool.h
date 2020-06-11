#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <mutex>
#include <math.h>
#include <cmath>
#include <vector>


#define GETMEMORY(CLASS) (CLASS##*)MemoryManager::GetInstancePtr()->GetFreeMemory(sizeof(CLASS))
#define GETMEMORYBYSIZE(SIZE) MemoryManager::GetInstancePtr()->GetFreeMemory(SIZE)
#define FREEMEMORY MemoryManager::GetInstancePtr()->FreeMemory(sizeof(this),(char*)this)
#define PTRSIZE 8

#define PAGESIZE 20

#define AUTOMUTEX std::lock_guard<std::mutex> lock(m_mutex);

struct MemoryData
{
	char* m_pAddr = NULL;
	bool m_isUsed = false;
	MemoryData* m_pNext = NULL;
};


struct MemoryPool
{
	char* GetFreeMemory();

	void CapacityMemory();

	bool FreeMemory(char* addr);

	MemoryData* m_freeData = NULL;

	uint32_t m_len = 0;

	uint32_t m_count = 0;

	char* m_memoryAddr = nullptr;

};

class MemoryManager
{
public:

	void Init();

	void MemoryPool_init(uint32_t colume_no, uint32_t block_len[], uint32_t block_count[]);

	char* GetFreeMemory(uint32_t nsize);

	bool FreeMemory(uint32_t nsize, char* addr);

	static MemoryManager* GetInstancePtr();

protected:



private:

	MemoryManager() {}

	std::vector<MemoryPool*>m_MempoolVec;

	std::mutex m_mutex;

	uint32_t useCount = 0;

	uint32_t freeCount = 0;

};

#endif