#include <arec.h>

/* function identifiers */
#define F_INSTALL	1
#define F_WIPE		2
#define F_BACKUP	3
#define F_RESTORE	4
#define F_CLS		5
#define F_EXIT		6

extern int cmd_loop_handler();
extern int cmdparse(char *cmd, int len, char **outargs);
extern char *getstrline(int *len);