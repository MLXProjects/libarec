#ifndef AREC_NOSERVER
#include <arec_internal.h>

/******************************************************************************
 * NAME:
 *      arec_sv_recv_data
 *
 * DESCRIPTION: 
 *      Read data from socket fd.
 *
 * PARAMETERS:
 *      sockfd - The socket fd
 *      buf    - The buffer to keep data
 *      len    - The length of buffer
 *      flags  - Flags pass to recv() function
 *
 * RETURN:
 *      Bytes received.
 ******************************************************************************/
static ssize_t arec_sv_recv_data(int sockfd, char *buf, ssize_t len, int flags)
{
    ssize_t bytes;
    ssize_t pos;
    int count;
    fd_set readfds, writefds;
    struct timeval timeout;


    pos = 0;
    do {
        bytes = recv(sockfd, buf+pos, len-pos, flags);
        if (bytes < 0) {
            RLOGE("recv error");
            break;
        } else if (bytes == 0) {
            /* No data left, jump out */
            break;
        } else {
            pos += bytes;
            if (pos >= len) {
                /* The buffer is full, jump out */
                break;
            }
        }

        /*
         * Check if data is available from socket fd, count is 0 when no data
         * available. Make select wait up to 10 ms for incoming data.
         * NOTES: On Linux, select() modifies timeout to reflect the amount of time
         * not slept.
         */
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(sockfd, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 10*1000;
        count = select(sockfd + 1, &readfds, &writefds, (fd_set *)0, &timeout);
        if (count <= 0)  {
            break;
        }
    } while (count > 0);

    return pos;
}


#if 0

/******************************************************************************
 * NAME:
 *      recv_packet
 *
 * DESCRIPTION: 
 *      Receive a command packet. Use the data_len field in the header to avoid
 *      "no message boundaries" issue in "SOCK_STREAM" type socket.
 *        (1) Receive the header of packet
 *        (2) Check the signature of packet
 *        (3) Get the data length of packet
 *        (4) Receive the data of packet
 *
 * PARAMETERS:
 *      sockfd - The socket fd
 *      buf    - The buffer to keep command packet
 *      len    - The length of buffer
 *      flags  - Flags pass to recv() function
 *
 * RETURN:
 *      Bytes received.
 ******************************************************************************/
static ssize_t recv_packet(int sockfd, char *buf, ssize_t len, int flags)
{
    ssize_t bytes;
    ssize_t pos;
    int count;
    fd_set readfds, writefds;
    struct timeval timeout;
    arec_sv_cmd_t *pkt = (arec_sv_cmd_t *)buf;
    ssize_t header_len = sizeof(arec_sv_cmd_t);

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(sockfd, &readfds);

    /* Receive the header of command packet first */
    pos = 0;
    while (pos < header_len) {
        bytes = recv(sockfd, buf+pos, header_len-pos, flags);
        if (bytes < 0) {
            RLOGE("recv error");
            return 0;
        } else if (bytes == 0) {
            /* No data left, jump out */
            return pos;
        } else {
            pos += bytes;
            if (pos >= header_len) {
                /* All data of header received, jump out */
                break;
            }
        }

        /*
         * Check if data is available from socket fd, count is 0 when no data
         * available.
         * Make select wait up to 10 ms for incoming data.
         * NOTES: On Linux, select() modifies timeout to reflect the amount of time
         * not slept.
         */
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(sockfd, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 10*1000;
        count = select(sockfd + 1, &readfds, &writefds, (fd_set *)0, &timeout);
        if (count <= 0)  {
            break;
        }
    } 

    /* Check the signature of command packet */
    if (pkt->signature != AREC_SV_PKSIGNATURE) {
        printf("Error: invalid signature of packet\n");
        return 0;
    }

    /* Get the total length of command packet */
    if (len > pkt->data_len + header_len) {
        len = pkt->data_len + header_len;
    }

    /* Receive all data of command packet */
    while (pos < len) {
        bytes = recv(sockfd, buf+pos, len-pos, flags);
        if (bytes < 0) {
            RLOGE("recv error");
            break;
        } else if (bytes == 0) {
            /* No data left, jump out */
            break;
        } else {
            pos += bytes;
            if (pos >= len) {
                /* All data received, jump out */
                break;
            }
        }

        /*
         * Check if data is available from socket fd, count is 0 when no data
         * available.
         * Make select wait up to 10 ms for incoming data.
         * NOTES: On Linux, select() modifies timeout to reflect the amount of time
         * not slept
         */
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(sockfd, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 10*1000;
        count = select(sockfd + 1, &readfds, &writefds, (fd_set *)0, &timeout);
        if (count <= 0)  {
            break;
        }
    }

    return pos;
}
#endif


/******************************************************************************
 * NAME:
 *      arec_sv_compute_checksum
 *
 * DESCRIPTION: 
 *      Compute 16-bit One's Complement sum of data. (The algorithm comes from
 *      RFC-1071)
 *      NOTES: Before call this function, please set the checksum field to 0.
 *
 * PARAMETERS:
 *      buf  - The data buffer
 *      len  - The length of data(bytes).
 *
 * RETURN:
 *      Checksum
 ******************************************************************************/
static uint16_t arec_sv_compute_checksum(void *buf, ssize_t len)
{
    uint16_t *word;
    uint8_t *byte;
    ssize_t i;
    unsigned long sum = 0;

    if (!buf) {
        return 0;
    }

    word = (uint16_t *)buf;
    for (i = 0; i < len/2; i++) {
        sum += word[i];
    }

    /* If the length(bytes) of data buffer is an odd number, add the last byte. */
    if (len & 1) {
        byte = (uint8_t *)buf;
        sum += byte[len-1];
    }

    /* Take only 16 bits out of the sum and add up the carries */
    while (sum>>16) {
        sum = (sum>>16) + (sum&0xFFFF);
    }

    return (uint16_t)(~sum);
}


/******************************************************************************
 * NAME:
 *      arec_sv_verify_command_packet
 *
 * DESCRIPTION: 
 *      Verify the data integrity of the command packet.
 *
 * PARAMETERS:
 *      buf  - The data of command packet
 *      len  - The length of data
 *
 * RETURN:
 *      1 - OK, 0 - FAIL
 ******************************************************************************/
static int arec_sv_verify_command_packet(void *buf, size_t len)
{
    arec_sv_cmd_t *pkt;
    
    if (buf == NULL) {
        return 0;
    }
    pkt = (arec_sv_cmd_t *)buf;
    
    if (pkt->signature != AREC_SV_PKSIGNATURE) {
        printf("Error: invalid signature of packet (0x%08X)\n", pkt->signature);
        return 0;
    }

    if (pkt->data_len + sizeof(arec_sv_cmd_t) != len) {
        printf("Error: invalid length of packet (%ld:%ld)\n",
            pkt->data_len + sizeof(arec_sv_cmd_t), len);
        return 0;
    }

    if (arec_sv_compute_checksum(buf, len) != 0) {
        printf("Error: invalid checksum of packet\n");
        return 0;
    }

    return 1;
}


/******************************************************************************
 * NAME:
 *      arec_sv_reqhandler_routine
 *
 * DESCRIPTION: 
 *      A thread function to receive the request from client,
 *      handle the request and send response back to client.
 *
 * PARAMETERS:
 *      arg - A pointer of connection info
 *
 * RETURN:
 *      None
 ******************************************************************************/
static void *arec_sv_reqhandler_routine(void *arg)
{
    arec_sv_connect_t *sc = (arec_sv_connect_t *)arg;
    arec_sv_cmd_t *req;
    arec_sv_cmd_t *resp;
    uint8_t buf[AREC_SV_BUF_SIZE];
    ssize_t bytes, req_len, resp_len;

    if (sc == NULL) {
        printf("Error: invalid argument of thread routine\n");
        pthread_exit(0);
    }

    while (1) {
        /* Receive request from client */
        //req_len = recv(sc->client_fd, buf, sizeof(buf), 0);
        req_len = arec_sv_recv_data(sc->client_fd, (char *)buf, sizeof(buf), 0);
        if (req_len <= 0) {
            close(sc->client_fd);
            sc->inuse = 0;
            break;
        }

        /* Check the integrity of the request packet */
        if (!arec_sv_verify_command_packet(buf, req_len)) {
            /* Discard invaid packet */
            continue;
        }

        /* Process the request */
        req = (arec_sv_cmd_t *)buf;
        resp = sc->serv->request_handler(req);
        if (resp == NULL) {
            resp = (arec_sv_cmd_t *)buf;   /* Use a local buffer */
            resp->status = AREC_SV_CMDSTATUS_ERROR;
            resp->data_len = 0;
        }

        resp_len = sizeof(arec_sv_cmd_t) + resp->data_len;
        resp->signature = req->signature;
        resp->checksum = 0;
        resp->checksum = arec_sv_compute_checksum(resp, resp_len);

        /* Send response */
        bytes = send(sc->client_fd, resp, resp_len, MSG_NOSIGNAL);
        if (resp != (arec_sv_cmd_t *)buf) {    /* If NOT local buffer, free it */
            free(resp);
        }
        if (bytes != resp_len) {
            printf("Error: send response error\n");
            close(sc->client_fd);
            sc->inuse = 0;
            break;
        }
    }

    pthread_exit(0);
}


/******************************************************************************
 * NAME:
 *      arec_sv_init
 *
 * DESCRIPTION: 
 *      Do some initialzation work for server.
 *
 * PARAMETERS:
 *      sock_path - The path of unix domain socket
 *      req_handler - The function pointer of a user-defined request handler.
 *
 * RETURN:
 *      A pointer of server info.
 ******************************************************************************/
arec_sv_t *arec_sv_init(const char *sock_path, arec_sv_reqhandler_t req_handler)
{
    arec_sv_t *s;
    struct sockaddr_un addr;
    int i, rc;

    if (req_handler == NULL) {
        printf("Error: invalid parameter!\n");
        return NULL;
    }

    s = (arec_sv_t *)malloc(sizeof(arec_sv_t));
    if (s == NULL) {
        RLOGE("malloc error");
        return NULL;
    }
    memset(s, 0, sizeof(arec_sv_t));
    for (i = 0; i < AREC_SV_MAX_CLIENTS; i++) {
        s->conn[i].serv = s;
    }

    /* Setup request handler */
    s->request_handler = req_handler;

    unlink(sock_path);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, sock_path);

    s->sockfd = socket(AF_UNIX, AREC_SV_SOCK_TYPE, 0);
    if (s->sockfd < 0) {
        RLOGE("socket error");
        free(s);
        return NULL;
    }

    // Avoid "Address already in use" error in bind()
    //int val = 1;
    //if (setsockopt(s->sockfd, SOL_SOCKET, SO_REUSEADDR, &val,
    //        sizeof(val)) == -1) 
	//	{
    //    RLOGE("setsockopt error");
    //    return NULL;
	//	}
    // */
	

    rc = bind(s->sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if (rc != 0) {
        RLOGE("bind error");
        close(s->sockfd);
        free(s);
        return NULL;
    }

    rc = listen(s->sockfd, AREC_SV_MAX_BACKLOG);
    if (rc != 0) {
        RLOGE("listen error");
        close(s->sockfd);
        free(s);
        return NULL;
    }

    return s;
}


/******************************************************************************
 * NAME:
 *      arec_sv_accept_req
 *
 * DESCRIPTION: 
 *      Accept a request from client and start a new thread to process it.
 *
 * PARAMETERS:
 *      s - A pointer of server info
 *
 * RETURN:
 *      0 - OK, Others - Error
 ******************************************************************************/
int arec_sv_accept_req(arec_sv_t *s)
{
    arec_sv_connect_t *sc;
    int cl, i;

    if (s == NULL) {
        printf("Error: invalid parameter!\n");
        return -1;
    }

    cl = accept(s->sockfd, NULL, NULL);
    if (cl < 0) {
        RLOGE("accept error");
        return -1;
    }

    /* Find a slot for the connection */
    for (i = 0; i < AREC_SV_MAX_CLIENTS; i++) {
        if (!s->conn[i].inuse) {
            break;
        }
    }
    if (i >= AREC_SV_MAX_CLIENTS) {
        printf("Error: too many connections\n");
        close(cl);
        return -1;
    }

    /* Start a new thread to handle the request */
    sc = &s->conn[i];
    sc->inuse = 1;
    sc->client_fd = cl;
    if (pthread_create(&sc->thread_id, NULL, arec_sv_reqhandler_routine, sc) != 0) {
        RLOGE("pthread_create error");
        close(cl);
        sc->inuse = 0;
        return -1;
    }

    return 0;
}


/******************************************************************************
 * NAME:
 *      arec_sv_close
 *
 * DESCRIPTION: 
 *      Close the socket fd and free memory.
 *
 * PARAMETERS:
 *      s - A pointer of server info
 *
 * RETURN:
 *      None
 ******************************************************************************/
void arec_sv_close(arec_sv_t *s)
{
	if (arecovery()->onloop) arecovery()->onloop=0;
    int i;

    RLOGI("Server closing");

    if (s == NULL) {
        return;
    }

    for (i = 0; i < AREC_SV_MAX_CLIENTS; i++) {
        if (s->conn[i].inuse) {
            pthread_join(s->conn[i].thread_id, NULL);
            close(s->conn[i].client_fd);
        }
    }

    close(s->sockfd);
    free(s);
}

/*
 * When user press CTRL+C, quit the server process.
 */
void _arec_sigint_handler(int sig)
{
    arecovery()->onloop=0;
}

void arec_sv_enable_sigint_handler()
{
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_handler = _arec_sigint_handler;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);
}

/*
 * Get a message string from server
 */
arec_sv_cmd_t *_arec_sv_cmd_curtask(void)
{
    arec_sv_response_getmsg_t *res;
    //const char *str = "This is a message from the server.";
	//char *str = malloc(64);

    RLOGI("AREC_CMD_CURRENT_TASK");

    res = (arec_sv_response_getmsg_t *)malloc(sizeof(arec_sv_response_getmsg_t));
    if (res != NULL) {
        res->common.status = AREC_SV_CMDSTATUS_SUCCESS;
        res->common.data_len = 48;
        snprintf(res->data, AREC_SV_MAX_MSG_SIZE, "%s: recovery %s running", arec_version(AREC_REQVERSION_FULL), (arecovery()->onloop)?"is":"isn't");
        res->data[AREC_SV_MAX_MSG_SIZE-1] = 0;
    }

    return (arec_sv_cmd_t *)res;
}

/*
 * Unknown request type
 */
arec_sv_cmd_t *_arec_sv_cmd_unknown(arec_sv_cmd_t *req)
{
    arec_sv_cmd_t *res;

    RLOGI("Unknown request type");

    res = (arec_sv_cmd_t *)malloc(sizeof(arec_sv_cmd_t));
    if (res != NULL) {
        res->status = AREC_SV_CMDSTATUS_INVALID;
        res->data_len = 0;
    }

    return res;
}

/*
 * The handler to manage all client requests
 */
arec_sv_cmd_t *arec_sv_reqhandler(arec_sv_cmd_t *req)
{
	arec_sv_cmd_t *resp = NULL;

	switch (req->command) {
	case AREC_CMD_CURRENT_TASK:
		resp = _arec_sv_cmd_curtask();
		break;
	/*
	case AREC_CMD_PROGRESS:
		resp = _arec_sv_cmd_progress();
		break;
		
	case AREC_CMD_INSTALL:
		resp = _arec_sv_cmd_install(req);
		break;
		
	case AREC_CMD_BACKUP:
		resp = _arec_sv_cmd_backup(req);
		break;
		
	case AREC_CMD_RESTORE:
		resp = _arec_sv_cmd_restore(req);
		break;
		
	case AREC_CMD_WIPE:
		resp = _arec_sv_cmd_wipe(req);
		break;
		
		
	case AREC_CMD_ABORT_CURRENT:
		resp = _arec_sv_cmd_abort();
		break;
		*/
	default:
		resp = _arec_sv_cmd_unknown(req);
		break;
	}

	return resp;
}

/******************************************************************************
 * NAME:
 *      client_init
 *
 * DESCRIPTION: 
 *      Init client and connect to the server
 *
 * PARAMETERS:
 *      sock_path - The path of unix domain socket
 *      timeout   - Wait the server to be ready(in seconds)
 *
 * RETURN:
 *      A pointer of client info.
 ******************************************************************************/
arec_sv_client_t *arec_sv_client_init(int timeout)
{
    arec_sv_client_t *sc;
    struct sockaddr_un addr;
    int fd, rc;

    sc = (arec_sv_client_t *)malloc(sizeof(arec_sv_client_t));
    if (sc == NULL) {
        RLOGE("client_init: malloc error");
        return NULL;
    }
    memset(sc, 0, sizeof(arec_sv_client_t));

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, AREC_SV_SOCKET_PATH);
    fd = socket(AF_UNIX, AREC_SV_SOCK_TYPE, 0);
    if (fd < 0) {
        RLOGE("client_init: socket error");
        free(sc);
        return NULL;
    }
    sc->sockfd = fd;

    do {
        rc = connect(sc->sockfd, (struct sockaddr *)&addr, sizeof(addr));
        if (rc == 0) {
            break;
        } else {
            arec_sleep(1000);
        }
    } while (timeout-- > 0);
    if (rc != 0) {
        RLOGE("client_init: connect error");
        close(sc->sockfd);
        free(sc);
        return NULL;
    }

    return sc;
}


/******************************************************************************
 * NAME:
 *      client_send_request
 *
 * DESCRIPTION: 
 *      Send a request to server, and get the response.
 *
 * PARAMETERS:
 *      c   - A pointer of client info
 *      req - The request to send
 *
 * RETURN:
 *      The response for the request. The caller need to free the memory.
 ******************************************************************************/
arec_sv_cmd_t *arec_sv_client_send_request(arec_sv_client_t *c, arec_sv_cmd_t *req)
{
    uint8_t buf[AREC_SV_BUF_SIZE];
    ssize_t bytes, req_len;

    if ((c == NULL) || (req == NULL)) {
        RLOGE("client_send_request: invalid parameter!");
        return NULL;
    }

    /* Send request */
    req_len = sizeof(arec_sv_cmd_t) + req->data_len;
    req->signature = AREC_SV_PKSIGNATURE;
    req->checksum = 0;
    req->checksum = arec_sv_compute_checksum(req, req_len);
    bytes = send(c->sockfd, req, req_len, MSG_NOSIGNAL);
    if (bytes != req_len) {
        RLOGE("client_send_request: send error");
        return NULL;
    }

    /* Get response */
    bytes = arec_sv_recv_data(c->sockfd, (char *)buf, sizeof(buf), 0);
    if (bytes <= 0) {
        RLOGE("client_send_request: receive response error");
        return NULL;
    }

    if (arec_sv_verify_command_packet(buf, bytes)) {
        arec_sv_cmd_t *resp = (arec_sv_cmd_t *)malloc(bytes);
        if (resp) {
            memcpy(resp, buf, bytes);
        } else {
            RLOGE("client_send_request: malloc error");
        }
        return resp;
    }

    return NULL;
}


/******************************************************************************
 * NAME:
 *      client_close
 *
 * DESCRIPTION: 
 *      Close the client socket fd and free memory.
 *
 * PARAMETERS:
 *      c - The pointer of client info
 *
 * RETURN:
 *      None
 ******************************************************************************/
void arec_sv_client_close(arec_sv_client_t *c)
{
    if (c == NULL) {
        return;
    }

    close(c->sockfd);
    free(c);
}

#endif /* AREC_NOSERVER */