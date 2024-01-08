# TCP Socket

Implementation of a TCP socket API that wraps Windows and Linux sockets.

### Usage

```c++
#include "tcp_socket.h"

using namespace tcp_socket;

int main() {
	tcp_socket::init();

	// bind
	TCP_SOCKET listen_socket;

	if (!tcp_socket::bind(8080, listen_socket)) {
		printf("Failed to bind socket : %s\n", tcp_socket::last_error.c_str());
		tcp_socket::free();
		return 1;
	}

	// listen
	if (!tcp_socket::listen(listen_socket)) {
		printf("Failed to listen : %s\n", tcp_socket::last_error.c_str());
		tcp_socket::free();
		return 1;
	}

	// accept
	TCP_SOCKET client_socket;

	if (!tcp_socket::accept(listen_socket, client_socket)) {
		printf("Failed to accept socket connection : %s\n", tcp_socket::last_error.c_str());
		tcp_socket::close(listen_socket);
		tcp_socket::free();
		return false;
	}

	// send
	std::string message = "Hello, sailor!";
	tcp_socket::send(client_socket, message.c_str(), message.size());

	// receive
	char buffer[1024];
	int num_bytes = tcp_socket::receive(client_socket, buffer, 1024);

	// free
	tcp_socket::close(client_socket);
	tcp_socket::close(listen_socket);

	tcp_socket::free();

	return 0;
}

```

