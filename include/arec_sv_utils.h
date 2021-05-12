/************************************/
/*       COPYRIGHT&LICENSING:       */
/*   SERVER/CLIENT IMPLEMENTATION   */
/*   BASED OFF Shengkui Leng CODE   */
/* https://github.com/shengkui/uds/ */
/*    LICENSED UNDER APACHE 2.0     */
/*                                  */
/************************************/
#ifndef AREC_NOSERVER
#ifndef __arec_sv_h__
#define __arec_sv_h__
#include <arec.h>
#include <arec_internal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

/* The socket type we used */
#define AREC_SV_SOCK_TYPE           SOCK_STREAM
//#define UDS_SOCK_TYPE         SOCK_SEQPACKET

/* The read/write buffer size of socket */
#define AREC_SV_BUF_SIZE            1024

/* The signature of the request/response packet */
#define AREC_SV_PKSIGNATURE           0xDEADBEEF

/* Make a structure 1-byte aligned */
#define BYTE_ALIGNED            __attribute__((packed))


/* Status code, the values used in struct uds_command_t.status */
#define AREC_SV_CMDSTATUS_SUCCESS		0	/* Success */
#define AREC_SV_CMDSTATUS_ERROR			1	/* Generic error */
#define AREC_SV_CMDSTATUS_INVALID		2	/* Invalid command */

/* The maximum length of the queue of pending connections */
#define AREC_SV_MAX_BACKLOG     10

/* The maxium count of client connected */
#define AREC_SV_MAX_CLIENTS      10

enum arec_sv_cmd_type {
    AREC_CMD_INSTALL,			// install list of packages
	AREC_CMD_BACKUP,			// backup selected partitions
	AREC_CMD_RESTORE,			// restore selected partitions
	AREC_CMD_WIPE,				// wipe selected partitions
	AREC_CMD_CURRENT_TASK,		// get current task type
	AREC_CMD_PROGRESS,			// request current task progress
	AREC_CMD_ABORT_CURRENT		// request to stop current task
};

/* Common header of both request/response packets */
typedef struct arec_sv_cmd {
    uint32_t signature;         /* Signature, shall be AREC_SV_PKSIGNATURE */
    union {
        uint32_t command;       /* Request type */
        uint32_t status;        /* Status code of response */
    };
    uint32_t data_len;          /* The data length of packet */

    uint16_t checksum;          /* The checksum of the packet */
} BYTE_ALIGNED arec_sv_cmd_t;

typedef arec_sv_cmd_t * (*arec_sv_reqhandler_t) (arec_sv_cmd_t *);

/* Keep the information of connection */
typedef struct arec_sv_connect {
    int inuse;					/* 1: the connection structure is in-use; 0: free */
    int client_fd;				/* Socket fd of the connection */
    pthread_t thread_id;		/* The thread id of request handler */
    struct arec_sv *serv;		/* The pointer of arec_sv who own the connection */
} arec_sv_connect_t;

typedef struct arec_sv {
    int sockfd;									/* Socket fd of the server */
    arec_sv_connect_t conn[AREC_SV_MAX_CLIENTS];/* Connections managed by server */
    arec_sv_reqhandler_t request_handler;		/* Function pointer of the request handle */
} arec_sv_t;

/* Keep the information of client */
typedef struct arec_sv_client {
    int sockfd;         /* Socket fd of the client */
} arec_sv_client_t;

/* Response for CMD_GET_MESSAGE */
#define AREC_SV_GET_MSG_SIZE        256
typedef struct arec_sv_response_getmsg {
    arec_sv_cmd_t common;				/* Common header of response */
    char data[AREC_SV_GET_MSG_SIZE];	/* Data from server to client */
} BYTE_ALIGNED arec_sv_response_getmsg_t;

extern arec_sv_t *arec_sv_init(const char *sock_path, arec_sv_reqhandler_t req_handler);
extern int arec_sv_accept_req(arec_sv_t *s);
extern void arec_sv_close(arec_sv_t *s);
extern void arec_sv_enable_sigint_handler();
extern arec_sv_cmd_t *arec_sv_reqhandler(arec_sv_cmd_t *req);

//client things
extern arec_sv_client_t *arec_sv_client_init(int timeout);
extern arec_sv_cmd_t *arec_sv_client_send_request(arec_sv_client_t *c, arec_sv_cmd_t *req);
extern void arec_sv_client_close(arec_sv_client_t *s);

#endif /* __arec_sv_h__ */
#endif /* AREC_NOSERVER */