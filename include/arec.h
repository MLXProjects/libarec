#ifndef __arec_h__
#define __arec_h__

/*********LIBAREC - Android RECovery LIBrary*********/
/*													*/
/*	Use this respecting the Apache 2.0 License pls	*/
/*													*/
/*		  Pieces of code taken from LIBAROMA		*/
/*				 by Ahmad Amarullah: 				*/
/*		 https://github.com/amarullz/libaroma		*/
/*													*/
/*		 Pieces of code taken from UDS sample		*/
/*				  by Shengkui Leng:	 				*/
/*		   https://github.com/shengkui/uds			*/
/*													*/
/****************************************************/

//#include <aroma.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef AREC_NOSERVER
	#include <arec_info.h>
#else 
	#define AREC_SV_SOCKET_PATH	"\0arec_sv"
#endif
/* version request options */
#define AREC_REQVERSION_FULL	1
#define AREC_REQVERSION_NUMBER	2
#define AREC_REQVERSION_MAJOR	3
#define AREC_REQVERSION_MINOR	4
#define AREC_REQVERSION_PATCH	5

/* return codes for install */
#define AREC_INSTALL_SERVER_WAIT	2
#define AREC_INSTALL_SUCCESS		1
#define AREC_INSTALL_FAILED			0
#define AREC_INSTALL_NOFILES		-1

typedef struct {
	/* settings manager */
	//RECCONFIGSP config;
#ifndef AREC_NOSERVER
	/* server info for IPC */
	int server_fd;
	int client_fd;
#endif
	
	/* current process info */
	int is_installing;
	char *current_zip;
	int onloop;	
	
} ARECOVERY, * ARECOVERYP;

extern ARECOVERYP arecovery();
extern char *arec_version(int type);
#ifdef AREC_NOSERVER
extern char *arec_version();
#else
extern ARECOVERYP *arec_connect();
extern void *arec_send_request(int cmd, char *data);
#endif
extern int arec_install(char *paths);
extern int arec_wipe(char *partition);

#endif /* __arec_h__ */