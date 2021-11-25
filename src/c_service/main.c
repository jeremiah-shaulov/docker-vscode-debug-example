// Asynchronous echo-server implemented in C. Based on libev.
// To compile for production:
//	clang -DNDEBUG -O2 main.c -lev -o main
// To compile for debug:
//	clang -O0 -g main.c -lev -o main

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <ev.h>

#define S(s) s, sizeof(s)

enum
{	PORT = 8543,
	LISTEN_BACKLOG = 128,
	BUFFER_SIZE = 1024,
};

typedef void (*async_cb)(void *cb_arg, int result);

// Server socket descriptor.
// This is global singleton assign-once variable in single-threaded application.
int server_fd;


// ---------------------------------------------
// Declare async_recv().
//
// This function calls recv(fd, buffer, buffer_len), and when the result is ready, it calls the given callback function.
// The callback will receive `cb_arg` and integer result, that indicates how many bytes were received.
// If `result` is 0, this means that EOF reached.
// If `result` is -1, this means that error occured.
// ---------------------------------------------

// I'll pass this struct to ev_io_init() instead of ev_io, so it will think that it's ev_io (because ev_io is the first field) and i'll be able to reach more fields of this structure.
struct async_recv_t
{	ev_io io;
	int fd;
	char *buffer;
	int buffer_len;
	async_cb cb;
	void *cb_arg;
};
static void async_recv_sub(struct ev_loop *loop, ev_io *io, int revents)
{	struct async_recv_t* priv = (struct async_recv_t *)io; // it was not just address of ev_io, but of async_recv_t (see above)

	ev_io_stop(loop, io);

	int recv_len = recv(priv->fd, priv->buffer, priv->buffer_len, 0);
	priv->cb(priv->cb_arg, recv_len);
	free(priv);
}
void async_recv(struct ev_loop *loop, int fd, char *buffer, int buffer_len, async_cb cb, void *cb_arg)
{	struct async_recv_t *priv = malloc(sizeof(struct async_recv_t));
	priv->fd = fd;
	priv->buffer = buffer;
	priv->buffer_len = buffer_len;
	priv->cb = cb;
	priv->cb_arg = cb_arg;
	ev_io_init((ev_io *)priv, async_recv_sub, fd, EV_READ);
	ev_io_start(loop, (ev_io *)priv);
}


// ---------------------------------------------
// Declare async_send().
//
// This function calls send(fd, data, data_len), and when all the bytes were sent, or error occured, it calls the given callback function.
// The callback will receive `cb_arg` and integer result.
// If `result` is -1, this means that error occured. Other values for `result` are meaningless.
// ---------------------------------------------

// I'll pass this struct to ev_io_init() instead of ev_io, so it will think that it's ev_io (because ev_io is the first field) and i'll be able to reach more fields of this structure.
struct async_send_t
{	ev_io io;
	int fd;
	char *data;
	int data_len;
	async_cb cb;
	void *cb_arg;
};
static void async_send_sub(struct ev_loop *loop, ev_io *io, int revents)
{	struct async_send_t* priv = (struct async_send_t *)io; // it was not just address of ev_io, but of async_send_t (see above)

	int sent_len = send(priv->fd, priv->data, priv->data_len, 0);
	if (sent_len < priv->data_len && sent_len >= 0)
	{	priv->data += sent_len;
		priv->data_len -= sent_len;
	}
	else
	{	ev_io_stop(loop, io);
		priv->cb(priv->cb_arg, sent_len);
		free(priv);
	}
}
void async_send(struct ev_loop *loop, int fd, char *data, int data_len, async_cb cb, void *cb_arg)
{	struct async_send_t *priv = malloc(sizeof(struct async_send_t));
	priv->fd = fd;
	priv->data = data;
	priv->data_len = data_len;
	priv->cb = cb;
	priv->cb_arg = cb_arg;
	ev_io_init((ev_io *)priv, async_send_sub, fd, EV_WRITE);
	ev_io_start(loop, (ev_io *)priv);
}


// ---------------------------------------------
// Declare client_t and it's methods.
//
// This object has all the logic on how to handle an incoming connection.
//
// First create the object with `client_new(loop, fd)`.
// Then call `handle_conn(client, 0)` to handle the connection accepted from the server socket.
// `handle_conn()` will serve the request, and at last it will `close()` it's `fd`, and delete the object.
// ---------------------------------------------

enum CLIENT_STATE
{	CLIENT_STATE_INITIAL = 0,
	CLIENT_STATE_AFTER_SEND,
	CLIENT_STATE_AFTER_RECV,
	CLIENT_STATE_AFTER_SEND_LAST,
};
struct client_t
{	int state;
	struct ev_loop *loop;
	int fd;
	char buffer[BUFFER_SIZE];
};
struct client_t *client_new(struct ev_loop *loop, int fd)
{	struct client_t *client = calloc(1, sizeof(struct client_t));
	client->loop = loop;
	client->fd = fd;
	return client;
}
void client_delete(struct client_t *client)
{	close(client->fd);
	free(client);
}
void handle_conn(struct client_t *client, int result)
{	switch (client->state)
	{	case CLIENT_STATE_INITIAL:
		{	puts("Conn");
			async_send(client->loop, client->fd, S("Hi, i'm c_service\n"), (async_cb)handle_conn, client);
			client->state = CLIENT_STATE_AFTER_SEND;
			break;
		}
		case CLIENT_STATE_AFTER_SEND:
		{	if (result != -1)
			{	// Sent data successfully
				async_recv(client->loop, client->fd, client->buffer, BUFFER_SIZE, (async_cb)handle_conn, client);
				client->state = CLIENT_STATE_AFTER_RECV;
			}
			else
			{	// Error
				perror("send() failed");
				client_delete(client);
			}
			break;
		}
		case CLIENT_STATE_AFTER_RECV:
		{	if (result > 0)
			{	// Received data
				async_send(client->loop, client->fd, client->buffer, result, (async_cb)handle_conn, client);
				client->state = CLIENT_STATE_AFTER_SEND;
			}
			else if (result == 0)
			{	// Client disconnected
				async_send(client->loop, client->fd, S("Bye\n"), (async_cb)handle_conn, client);
				client->state = CLIENT_STATE_AFTER_SEND_LAST;
			}
			else
			{	// Error
				perror("recv() failed");
				client_delete(client);
			}
			break;
		}
		default:
		{	assert(client->state == CLIENT_STATE_AFTER_SEND_LAST);
			if (result == -1)
			{	perror("send() failed");
			}
			client_delete(client);
			break;
		}
	}
}


// ---------------------------------------------
// Server part.
// ---------------------------------------------

// Add O_NONBLOCK to given file descriptor
static int set_nonblock(int fd)
{	return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

// Callback for: server socket is ready to read
static void server_ready_cb(struct ev_loop *loop, ev_io *io, int revents)
{	// Accept
	int client_fd;
	while ((client_fd = accept(server_fd, NULL, NULL)) == -1)
	{	if (errno!=EAGAIN && errno!=EWOULDBLOCK)
		{	perror("accept() failed");
			exit(EXIT_FAILURE);
		}
	}
	if (set_nonblock(client_fd) == -1)
	{	perror("fcntl() failed");
		close(client_fd);
	}
	else
	{	handle_conn(client_new(loop, client_fd), 0);
	}
}

int main()
{	// Create server socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{	perror("socket() failed");
		return EXIT_FAILURE;
	}

	// Set the socket non-blocking
	if (set_nonblock(server_fd) == -1)
	{	perror("fcntl() failed");
		return EXIT_FAILURE;
	}

	// Bind
	struct sockaddr_in s;
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(INADDR_ANY);
	int err = bind(server_fd, (struct sockaddr *)&s, sizeof(s));
	if (err == -1)
	{	perror("bind() failed");
		return EXIT_FAILURE;
	}

	// Listen
	err = listen(server_fd, LISTEN_BACKLOG);
	if (err == -1)
	{	perror("listen() failed");
		return EXIT_FAILURE;
	}

	// Need 1 loop for single-thread application
	struct ev_loop *loop = EV_DEFAULT;

	// Get notified whenever the socket is ready to read (accept a connection)
	ev_io server_io;
	ev_io_init(&server_io, server_ready_cb, server_fd, EV_READ);
	ev_io_start(loop, &server_io);

	// Run event loop
	printf("Service started on %d\n", PORT);
	ev_loop(loop, 0);

	// Event loop exited
	close(server_fd);
	puts("Service terminated");
	return EXIT_SUCCESS;
}
