#include <arec_internal.h>

/* recovery variable */
static ARECOVERY _recovery={0};

#ifndef AREC_NOSERVER
/* client variable */
static arec_sv_client_t *_arec_client;
#endif

/* get recovery variable */
ARECOVERYP arecovery(){
	return &_recovery;
}

#ifndef AREC_NOSERVER

ARECOVERYP *arec_connect(){
	RLOGI("Connecting to server...");
	arec_sv_client_t *client = arec_sv_client_init(10);
	if (!client) {
		RLOGE("Unable to connect to server!");
		return NULL;
	}
	else RLOGE("Connected successfully :D");
	_arec_client=client;
	ARECOVERYP instance = malloc(sizeof(ARECOVERY));
	//gather data from server
	//_recovery.server_fd=;
	_recovery.client_fd=client->sockfd;
	return instance;
}

void *arec_send_request(int cmd, char *data){
	
	arec_sv_cmd_t req;
	arec_sv_response_getmsg_t *res;

	req.command = cmd;
	req.data_len = 0;

	res = (arec_sv_response_getmsg_t *)arec_sv_client_send_request(_arec_client, &req);
	if (res == NULL) {
		printf("client: send request error\n");
		arec_sv_client_close(_arec_client);
		return (void *)AREC_SV_CMDSTATUS_ERROR;
	}

	if (res->common.status == AREC_SV_CMDSTATUS_SUCCESS) {
		printf("Message: %s\n", res->data);
	} else {
		printf("client: CMD_GET_MESSAGE error(%d)\n", res->common.status);
	}
	
	return (void *)res->data;
}

#endif

void *_arec_install_thread(void *paths_pointer){
	char *paths=(char*)paths_pointer;
	int *count = NULL;
	char **packages = arec_parse_package_list(paths, &count);
	if (packages==NULL || count==0){
		return &AREC_INSTALL_NOFILES;
	}
	PPLOGI("Installing %d packages: \n", count);
	int pkg_i;
	for (pkg_i=0; pkg_i<*count; pkg_i++){
		PPLOGI("%s\n", packages[pkg_i]);
		arec_sleep(5000);
	}
	
#ifndef AREC_NOSERVER
	//tell clients that a package finished installing, and 
	//send the status (success/failed)
#endif
}

int arec_install(char *paths){
	//RLOGI("Call install here pls");
	int *ret=NULL;
#ifdef AREC_NOSERVER
	//standalone implementation
	ret = (int *)_arec_install_thread((void *)paths);
#else
	//client implementation
	pthread_t dummy;
	pthread_create(&dummy, NULL, _arec_install_thread, (void *)paths);
	ret=&AREC_INSTALL_SERVER_WAIT;
#endif
	return *ret;
}

char **arec_parse_package_list(char *string, int **count){
	if (!string) return NULL;
	if (string[0]=='\0') return NULL;
	PPLOGI("Starting!");
	/*	list: pointer to char array containing multiple paths
		path_idx: index of current path item being filled
		path_len: maximum path count allocated
		char_idx: index of current character inside of path
		char_len: maximum length of current path
		parse_idx: index of current string char
	*/
	char **list=NULL;
	int path_idx, path_len, parse_idx, char_idx, char_len, inside_quotes;
	path_idx=0;
	path_len=0;
	char_idx=0;
	char_len=0;
	inside_quotes=0;
	
	while (string[parse_idx]!='\0'){
		PPLOGI("Checking character %c", string[parse_idx]);
		if (path_idx==path_len) {
			PPLOGI("Reallocating to fit %d paths!", path_len+1);
			char **tmp_list=realloc(list, path_len++);
			if (tmp_list==NULL) {
				PPLOGE("Could not reallocate list!");
				int i;
				for (i=0; i<path_idx; i++)
					free(list[i]);
				free(list);
				return NULL;
			}
			char_len=0;
			list=tmp_list;
			
		}
		PPLOGI("Checking for double quotes");
		if (string[parse_idx]=='"'){
			PPLOGI("Quotes detected!");
			inside_quotes=!inside_quotes;
			if (!inside_quotes){
				list[path_idx][char_idx]='\0';
				path_idx++;
				char_idx=0;
			}
			parse_idx++;
			continue;
		}
		PPLOGI("Checking for space");
		if (string[parse_idx]==' '){
			if (char_idx==0) {
				//if first character in package, skip space
				parse_idx++;
				continue;
			}
			PPLOGI("Space detected!");
			//if should continue on same path, keep adding characters
			if (!inside_quotes){
				PPLOGI("space handler: jumping to next path!");
				list[path_idx][char_idx]='\0';
				path_idx++;
				char_idx=0;
				continue;
			}
		}
		PPLOGI("Checking for enough space");
		if (char_idx==char_len){
			PPLOGI("Reallocating to fit %d characters!", char_len+5);
			char *tmp_char=realloc(list[path_idx], char_len+5);
			if (tmp_char==NULL){
				PPLOGE("Could not reallocate path!");
				int i;
				for (i=0; i<path_idx; i++)
					free(list[i]);
				free(list);
				return NULL;
			}
			char_len+=5;
			list[path_idx]=tmp_char;
		}
		PPLOGI("Adding value %d to list[%d][%d]", parse_idx, path_idx, char_idx);
		list[path_idx][char_idx]=string[parse_idx];
		parse_idx++;
		char_idx++;
	}
	if (char_idx==0) path_idx--;
	*count=(int*)path_idx+1;
	PPLOGI("Successfully parsed %d packages!", *count);
	return list;
}