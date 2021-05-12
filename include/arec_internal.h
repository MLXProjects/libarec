#ifndef __arec_internal_h__
#define __arec_internal_h__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <arec.h>
#include <arec_debug.h>

#ifndef AREC_NOSERVER
#include <arec_sv_utils.h>

#define AREC_SV_MAX_MSG_SIZE	255
#endif

#define AREC_VERSION_MAJOR	0
#define AREC_VERSION_MINOR	1
#define AREC_VERSION_PATCH	0

typedef unsigned char byte;
typedef byte * bytep;
typedef void * voidp;

#define arec_sleep(ms) usleep(ms*1000)

/*typedef struct {
	
} RECCONFIG, * RECCONFIGP;*/

extern byte _arec_msg_loop();
extern char **arec_parse_package_list(char *string, int **count);

#endif /* __arec_internal_h__ */