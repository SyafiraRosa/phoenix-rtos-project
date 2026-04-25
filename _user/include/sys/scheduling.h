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

/* Step 27-28: Process group management */
#define PROCESS_GROUP_A 0  /* default: quanta x1 */
#define PROCESS_GROUP_B 1  /* quanta x2 */
#define PROCESS_GROUP_C 2  /* quanta x4 */

extern int setProcessGroup(pid_t pid, int group);
extern int getProcessGroup(pid_t pid);

#ifdef __cplusplus
}
#endif

#endif
