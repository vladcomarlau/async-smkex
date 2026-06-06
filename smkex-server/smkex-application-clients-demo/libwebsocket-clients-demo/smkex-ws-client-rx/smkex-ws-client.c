#include <libwebsockets.h>

#include <string.h>

// #define BASIC_AUTHENTICATION

static struct lws * client_wsi;

static int
callback_ws(struct lws * wsi, enum lws_callback_reasons reason,
	void * user, void * in , size_t len) {

  char *data = (char *) in;

	switch (reason) {
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		printf("Connection Error\n");
		client_wsi = NULL;
		break;

	case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
	#ifdef BASIC_AUTHENTICATION
		printf("Server application requires authentication\n");
		unsigned char **pp = (unsigned char **)in, *pend = (*pp) + len;
		char b[128];
		if (lws_http_basic_auth_gen("root", "smkex", b, sizeof(b)))
			break;
		if (lws_add_http_header_by_token(wsi,
						 WSI_TOKEN_HTTP_AUTHORIZATION,
						 (unsigned char *)b,
						 (int)strlen(b), pp, pend))
			return -1;
	#endif
		break;

	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		printf("Connection established\n");
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		printf("Received data with len %ld: %s\n", strlen(data), (const char * ) in );
		break;

	case LWS_CALLBACK_CLIENT_CLOSED:
		client_wsi = NULL;
		break;

	default:
		break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in , len);
}

static
const struct lws_protocols protocols[] = {
	{
		"ws",
		callback_ws,
		0,
		0,
	},
	{ NULL,	NULL,0,	0 }
};

static int receive_data(const char client_id[], const char dest_ip[], int dest_port) {
	struct lws_context_creation_info info;
	struct lws_client_connect_info i;
	struct lws_context * context;
  char wspath[256];

	const char * p;
	int n = 0;

	memset( & info, 0, sizeof info);

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.timeout_secs = 60;
	info.connect_timeout_secs = 60;
	info.fd_limit_per_thread = 3;

	context = lws_create_context( & info);
	if (!context)
		return 1;

	memset( & i, 0, sizeof i);
	i.context = context;
	i.address = dest_ip;
	i.port = dest_port;
	#ifdef BASIC_AUTHENTICATION
  	sprintf(wspath, "/private/smkex-websocket/%s", client_id);
  #else 
	//sprintf(wspath, "/public/smkex-websocket/%s", client_id);
  	sprintf(wspath, "/smkex-websocket/%s", client_id);

  #endif
  printf("wspath = %s\n", wspath);
	i.path = wspath; 
	i.host = i.address;
	i.origin = i.address;
	i.protocol = protocols[0].name;
	i.pwsi = & client_wsi;

	lws_client_connect_via_info( & i);

	while (n >= 0 && client_wsi)
		n = lws_service(context, 0);

	lws_context_destroy(context);

	return 0;
}

int main(int argc, const char ** argv) {

  char client_id[256];
  char dest_ip[256];
  int dest_port;

  if (argc < 4)
  {
    fprintf(stderr, "Too few arguments. Usage: %s {client_id} {dest_ip} {dest_port}\n", argv[0]);
    return 1;
  }

  strcpy(client_id, argv[1]);
  strcpy(dest_ip, argv[2]);
  dest_port = atoi(argv[3]);
	
	// Receive data from web socket 
	receive_data(client_id, dest_ip, dest_port);

	return 0;
}
