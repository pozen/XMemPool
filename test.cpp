/*
 * test.cpp
 *
 *  Created on: Mar 17, 2011
 *      Author: pozen
 */

#include "XMemPool.h"

int main()
{
	XMemPool samplePool(4096);
	XMemBlock *sampleBlock = samplePool.alloc(sizeof(int)*100);
	int *sampleData = sampleBlock->getMem<int>();
	samplePool.free(sampleBlock);
	return 0;
}


