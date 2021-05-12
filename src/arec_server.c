#include <arec_sv_utils.h>

static pthread_t receiver_loop_thread;
static char *_arec_getline_ex();
static int _arec_command_loop();
static arec_sv_t *_arec_sv_instance;

static void *receiver_loop(void *arg){
	while (arecovery()->onloop) {
		arec_sv_accept_req(_arec_sv_instance);
	}
}

int main(int argc, char **argv){
	RLOGI("startup: Initializing the server");
	_arec_sv_instance = NULL;
    _arec_sv_instance = arec_sv_init(AREC_SV_SOCKET_PATH, &arec_sv_reqhandler);
    if (_arec_sv_instance == NULL) {
        RLOGI("server: Init error");
        return 1;
    }
    arec_sv_enable_sigint_handler();
	arecovery()->onloop=1;
	
	RLOGI("statup: Creating connection handler thread");
	if (pthread_create(&receiver_loop_thread, NULL, receiver_loop, _arec_sv_instance) != 0){
		RLOGE("statup: Unable to create thread");
		arec_sv_close(_arec_sv_instance);
		return 1;
	}
	RLOGC("Welcome to Android Recovery server v%s", arec_version(AREC_REQVERSION_NUMBER));
    while (arecovery()->onloop) {
		if (!_arec_command_loop())
			break;
    }
	RLOGI("Closing the server");
	arec_sv_close(_arec_sv_instance);
	return 0;
}

static int _arec_command_loop(){
	printf(">");
	fflush(stdout);
	char *cmd = _arec_getline_ex();
	if (0==strcmp(cmd, "q") ||
		0==strcmp(cmd, "quit")){
			return 0;
	}
	else if (0==strcmp(cmd, "help")){
		printf("AREC SERVER v%s\n\n", arec_version(AREC_REQVERSION_NUMBER));
		printf("Commands: \n"
				"help - show this text\n"
				"q, quit, exit: close the server\n\n");
	}
	free(cmd);
	return 1;
}

static char *_arec_getline_ex(){
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
		//*length=j;
	}
	//else *length=i;
	return buffer;
}