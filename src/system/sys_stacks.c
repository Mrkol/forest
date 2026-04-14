#include "types.h"

#include "sys_stacks.h"

#ifdef TARGET_PC
ALIGNAS(4096) u8 graphStack[GRAPH_STACK_SIZE] ATTRIBUTE_ALIGN(4096);
ALIGNAS(4096) u8 padmgrStack [PADMGR_STACK_SIZE] ATTRIBUTE_ALIGN(4096);
ALIGNAS(4096) u8 irqmgrStack [IRQMGR_STACK_SIZE] ATTRIBUTE_ALIGN(4096);
#else
u8 graphStack[GRAPH_STACK_SIZE];
u8 padmgrStack [PADMGR_STACK_SIZE];
u8 irqmgrStack [IRQMGR_STACK_SIZE];
#endif
