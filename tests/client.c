#include <arec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ARECOVERYP recovery_instance;

int main(int argc, char **argv){
	printf("Starting client\n");
	recovery_instance = arec_connect(10);
	if (!recovery_instance) {
		printf("Unable to connect! \n");
		return 1;
	}
	arec_send_request(AREC_CMD_CURRENT_TASK);
	
	return 0;
}