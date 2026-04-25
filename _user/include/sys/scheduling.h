#ifndef _SYS_SCHEDULING_H_
#define _SYS_SCHEDULING_H_

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int setBaseQuanta(int quanta);
extern int getBaseQuanta(void);
extern int setQuantaForProcess(pid_t pid, int quanta);
extern int getQuantaForProcess(pid_t pid);

#ifdef __cplusplus
}
#endif

#endif
