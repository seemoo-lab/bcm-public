#include "../include/wrapper.h"

void
*dma_txfast_hook(void *di, void *p, int commit)
{
	
	return dma_txfast(di, p, commit);
}
