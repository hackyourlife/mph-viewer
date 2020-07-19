#ifndef __OS_H__
#define __OS_H__

void OS_Terminate(void);

#define	OS_Panic(...) { printf(__VA_ARGS__); printf("\n"); OS_Terminate(); }

#endif
