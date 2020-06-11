#include "memoryPool.h"


void MemoryManager::MemoryPool_init(uint32_t colume_no, uint32_t block_len[], uint32_t block_count[])
{
	for (int i = 0; i < colume_no; i++)
	{
		if (block_count[i] == 0 || block_len[i] == 0)
		{
			continue;
		}

		MemoryPool* pPool = new MemoryPool();
		pPool->m_count = block_count[i];
		pPool->m_len = block_len[i];
		pPool->m_memoryAddr = (char*)malloc((PTRSIZE + block_len[i]) * block_count[i]);
		memset(pPool->m_memoryAddr, 0, (PTRSIZE + block_len[i]) * block_count[i]);
		MemoryData* pMemoryData = new MemoryData();
		pPool->m_freeData = pMemoryData;
		pMemoryData->m_pAddr = pPool->m_memoryAddr;

		MemoryData* pTemp = pPool->m_freeData;
		for (uint32_t j = 1; j < block_count[i]; j++)
		{
			MemoryData* pNode = new MemoryData();
			pNode->m_pAddr = (pMemoryData->m_pAddr + (PTRSIZE + block_len[i]) * j);
			pTemp->m_pNext = pNode;

			pTemp = pNode;
		}

		m_MempoolVec.push_back(pPool);

	}
	return;
}

void MemoryManager::Init()
{
	uint32_t initlen = 4;
	uint32_t blockLen[PAGESIZE] = { 0 };
	uint32_t blockCount[PAGESIZE] = { 0 };
	for (uint32_t i = 0; i < PAGESIZE; i++)
	{
		blockLen[i] = initlen << i;
		blockCount[i] = PAGESIZE;
	}

	MemoryPool_init(PAGESIZE, blockLen, blockCount);
}

char* MemoryManager::GetFreeMemory(uint32_t nsize)
{
	AUTOMUTEX
		useCount++;

	if (nsize > 4)
	{
		return m_MempoolVec[ceil(log(nsize) / log(2)) - 2]->GetFreeMemory();
	}
	else
	{
		return m_MempoolVec[0]->GetFreeMemory();
	}
}

bool MemoryManager::FreeMemory(uint32_t nsize, char* addr)
{
	AUTOMUTEX
		freeCount++;

	if (nsize > 4)
	{
		return m_MempoolVec[ceil(log(nsize) / log(2)) - 2]->FreeMemory(addr);
	}
	else
	{
		return m_MempoolVec[0]->FreeMemory(addr);
	}
}

MemoryManager* MemoryManager::GetInstancePtr()
{
	static MemoryManager memoryManager;
	return &memoryManager;
}

char* MemoryPool::GetFreeMemory()
{
	m_freeData->m_isUsed = true;
	CapacityMemory();
	//std::cout << "GetFreeMemory1 : " << (void*)m_freeData << std::endl;
	uint64_t* ptr = (uint64_t*)m_freeData->m_pAddr;
	ptr[0] = (uint64_t)m_freeData;
	m_freeData = m_freeData->m_pNext;

	return (char*)&ptr[1];

}

void MemoryPool::CapacityMemory()
{
	if (m_freeData->m_pNext != NULL)
		return;

	//std::cout << "memory not enought m_len : "<< m_len << std::endl;

	char* addr = (char*)malloc((PTRSIZE + m_len) * m_count);
	if (addr == NULL)
	{
		printf("memory not enought \n");
		return;
	}

	MemoryData* pTemp = m_freeData;
	for (uint32_t i = 0; i < m_count; i++)
	{
		MemoryData* pNote = new MemoryData();
		pTemp->m_pNext = pNote;
		pNote->m_pAddr = addr + ((m_len + PTRSIZE) * i);
		pTemp = pNote;
	}

	m_count += m_count;
}

bool MemoryPool::FreeMemory(char* addr)
{
	//std::cout << "FreeMemory : " << (void*)addr << std::endl;
	uint64_t* ptr1 = (uint64_t*)(addr - 8);
	MemoryData* m_node = (MemoryData*)(ptr1[0]);
	//std::cout << "FreeMemory : " << (void*)m_node << std::endl;
	m_node->m_isUsed = false;
	m_node->m_pNext = m_freeData;
	m_freeData = m_node;
	return true;
}
