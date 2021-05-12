#include "test.h"

static ARECOVERYP instance;

int main(int argc, char **argv){
	int ret;
#ifdef AREC_NOSERVER
	printf("Welcome to %s :D\n", arec_version());
	ret = arec_start(&cmd_loop_handler);
#else
	//handle client-side connection, and start loop
	instance = arec_connect(10);
	if (!instance) {
		printf("Unable to connect! \n");
		return 1;
	}
	//arec_send_request(AREC_CMD_VERSION);
	while (instance->onloop){
		cmd_loop_handler();
	}
#endif
	return ret;
}

int cmd_loop_handler(){
	printf("client> "); //TODO: get client number
	fflush(stdout);
	int len;
	char *cmd = getstrline(&len);
	char *args = NULL;
	int command = cmdparse(cmd, len, &args);
	if (args==NULL)
	switch (command){
		case F_INSTALL:{
			switch (arec_install(args)){
				case AREC_INSTALL_SUCCESS:
					//printf("Finished.\n");
					break;
				case AREC_INSTALL_FAILED:
					//printf("Install failed!\n");
					break;
				case AREC_INSTALL_NOFILES:
					printf("Usage: install <path to zip files>\n");
					break;
			}
			goto ok;
		}
		case F_WIPE:{
			if (!args){
				printf("Usage: wipe <partition> || wipe <mountpoint>\n");
				goto ok;
			}
			/*printf("Going to wipe something!\n");
			printf("Wiping %s...\n", args);
			printf("Done\n");*/
			goto ok;
		}
		case F_CLS:{
			printf("\033c");
			goto ok;
		}
		case F_EXIT:{
			arec_end();
			goto ok;
		}
		case 0:
		default: 
			goto err;
	}
	if (len==0) goto ok;
	err:
	printf("%s: command not found\n", cmd);
	ok:
	free(cmd);
	return 1;
}

int cmdparse(char *cmd, int len, char **outargs){
	if (!cmd || !len) return 0;
	int ret = 0;
	if (len>=7){
		if (cmd[0]=='i' && cmd[1]=='n' && cmd[2]=='s' && cmd[3]=='t' && cmd[4]=='a' && cmd[5]=='l' && cmd[6]=='l'){
			ret=F_INSTALL;
			if (len>7)
				*outargs=cmd+8;
			goto end;
			
		}
		else if (cmd[0]=='r' && cmd[1]=='e' && cmd[2]=='s' && cmd[3]=='t' && cmd[4]=='o' && cmd[5]=='r' && cmd[6]=='e'){
			ret=F_RESTORE;
			if (len>7)
				*outargs=cmd+8;
			goto end;
		}
	}
	if (len>=6){
		if (cmd[0]=='b' && cmd[1]=='a' && cmd[2]=='c' && cmd[3]=='k' && cmd[4]=='u' && cmd[5]=='p'){
			ret=F_BACKUP;
			if (len>6)
				*outargs=cmd+7;
			goto end;
		}
	}
	if (len>=5){
		if (cmd[0]=='c' && cmd[1]=='l' && cmd[2]=='e' && cmd[3]=='a' && cmd[4]=='r'){
			ret=F_CLS;
			goto end;
		}
	}
	if (len>=4){
		if (cmd[0]=='w' && cmd[1]=='i' && cmd[2]=='p' && cmd[3]=='e'){
			ret=F_WIPE;
			if (len>4){
				//printf("wipe: args are %s\n", cmd+5);
				*outargs=cmd+5;
			}
			goto end;
		}
		else if (((cmd[0]=='e' && cmd[1]=='x') ||
				 (cmd[0]=='q' && cmd[1]=='u')) && 
				 cmd[2]=='i' && cmd[3]=='t'){
			ret=F_EXIT;
			goto end;
		}
	}
	if (len>=3){
		if (cmd[0]=='c' && cmd[1]=='l' && cmd[2]=='s'){
			ret=F_CLS;
			goto end;
		}
	}
	if (len>=1){
		if (cmd[0]=='q'){
			ret=F_EXIT;
			goto end;
		}
	}
	end:
	return ret;
}

char *getstrline(int *length){
	char *buffer = calloc(16, 1);
	int len=15, i=0, c;
	
	while (1){
		c = fgetc(stdin);
		if (c==EOF || c=='\n'){
			i++;
			break;
		}
		if (i==len){
			char *newb = realloc(buffer, len+16);
			if(newb==NULL){
				free(buffer);
				return NULL;
			}
			buffer=newb;
			//memset(buffer+len, 0, 16);
			len+=16;
		}
		buffer[i]=(char)c;
		i++;
	}
	buffer[i]='\0';
	//remove trailing spaces
	if (i>1){
		int j=i-1;
		while (buffer[j-1]==' '){
			j--;
			buffer[j]='\0';
			
		}
		*length=j;
	}
	else *length=i;
	return buffer;
}
