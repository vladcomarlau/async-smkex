#include <libwebsockets.h>
#include <string.h>

#define BUFFER_SIZE 1024

// #define BASIC_AUTHENTICATION

static char buf[LWS_PRE + BUFFER_SIZE];
static int buf_len;
static int bad, status;

static int
callback_http(struct lws *wsi, enum lws_callback_reasons reason,
	      void *user, void *in, size_t len)
{
	switch (reason) {
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		printf("Connection Error\n");
		bad = 1;
		lws_cancel_service(lws_get_context(wsi));
		break;

	case LWS_CALLBACK_CLOSED_CLIENT_HTTP:
		printf("Connection Closed\n");
		bad |= status != 200;
		lws_cancel_service(lws_get_context(wsi));
		break;

	case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
		printf("Connection Established\n");
		status = (int)lws_http_client_http_response(wsi);
		break;

	case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
		printf("Connection Completed\n");
		bad |= status != 200;
		lws_cancel_service(lws_get_context(wsi));
		break;


	case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
	#ifdef BASIC_AUTHENTICATION
		printf("Server application requires authentication\n");
		unsigned char **pp = (unsigned char **)in, *pend = (*pp) + len;
		char b[128];
		if (lws_http_basic_auth_gen("root", "smkex", b, sizeof(b)))
			break
		if (lws_add_http_header_by_token(wsi,
						 WSI_TOKEN_HTTP_AUTHORIZATION,
						 (unsigned char *)b,
						 (int)strlen(b), pp, pend))
			return -1;
	#else
			if (!lws_http_is_redirected_to_get(wsi)) {
			uint8_t** up = (uint8_t**) in;
			uint8_t* uend = *up + len - 1;
        
			lws_add_http_header_content_length(wsi, buf_len, up, uend);
			
			lws_client_http_body_pending(wsi, 1);
			lws_callback_on_writable(wsi);
		}
	#endif
		break;

	case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:
		printf("Send data via POST request\n");
		if (lws_http_is_redirected_to_get(wsi))
			break;
		
		lws_client_http_body_pending(wsi, 0);
		
		char *start = &buf[LWS_PRE];
		lws_write(wsi, start, buf_len, LWS_WRITE_HTTP_FINAL);

		return 0;

	default:
		break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static const struct lws_protocols protocols[] = {
	{
		"http",
		callback_http,
		0,
		0,
	},
	{ NULL, NULL, 0, 0 }
};

static int send_data(const char data[], const char client_id[], const char buddy_id[],
                     const char dest_ip[], int dest_port)
{
	struct lws_context_creation_info info;
	struct lws_client_connect_info i;
	struct lws_context *context;
  char wspath[256];
	int n = 0;
	bad = 0;
	status = 0;
	
	if (!data)
		return 1;
		
	memcpy(buf + LWS_PRE, data, strlen(data));
	buf_len = strlen(data);

	memset(&info, 0, sizeof info);

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.fd_limit_per_thread = 3;

	context = lws_create_context(&info);
	if (!context) {
		return 1;
	}

	memset(&i, 0, sizeof i);
	i.context = context;
	#ifdef BASIC_AUTHENTICATION
  	sprintf(wspath, "/private/send-message/%s/%s", client_id, buddy_id);
  #else
  	sprintf(wspath, "/public/send-message/%s/%s", client_id, buddy_id);
  #endif
  printf("wspath = %s\n", wspath);
	i.path = wspath;
  i.address = dest_ip;
	i.port = dest_port;
	i.host = i.address;
	i.origin = i.address;
	i.method = "POST";
	i.protocol = protocols[0].name;
	
	if (!lws_client_connect_via_info(&i))
		return 1;

	while (!bad && !status && n >= 0)
		n = lws_service(context, 0);

	lws_context_destroy(context);

	return bad;
}

int main(int argc, const char **argv)
{

	const char *data1;
	#ifdef BASIC_AUTHENTICATION
		data1 = "Hello from Private";
	#else
		data1 = "Hello from Public";
	#endif

  char client_id[256];
  char buddy_id[256];
  char dest_ip[256];
  int dest_port;

  if (argc < 5)
  {
    fprintf(stderr, "Too few arguments. Usage: %s {client_id} {buddy_id} {dest_ip} {dest_port}\n", argv[0]);
    return 1;
  }

  strcpy(client_id, argv[1]);
  strcpy(buddy_id, argv[2]);
  strcpy(dest_ip, argv[3]);
  dest_port = atoi(argv[4]);

	send_data(data1, client_id, buddy_id, dest_ip, dest_port);
	
	return 0;
}

