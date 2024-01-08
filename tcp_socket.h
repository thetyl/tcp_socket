// MIT License
// 
// Copyright(c) 2023 Tyl
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software andassociated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, andto permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice andthis permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <unordered_map>
#include <string>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#endif

namespace tcp_socket {

#ifdef _WIN32
	typedef SOCKET TCP_SOCKET;
#else
	typedef int TCP_SOCKET;
#endif

	std::string last_error;

	bool init() {
#ifdef _WIN32
		WSADATA wsa_data;

		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
			last_error = "WSAStartup error";
			return false;
		}
#endif

		return true;
	}

	void close(TCP_SOCKET sock) {
#ifdef _WIN32
		closesocket(sock);
#else
		::close(sock);
#endif
	}

	void free() {
#ifdef _WIN32
		WSACleanup();
#endif
	}

	bool bind(int port, TCP_SOCKET &_sock) {
		_sock = NULL;

		addrinfo hints = {};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
#ifdef _WIN32
		hints.ai_protocol = IPPROTO_TCP;
#else
		hints.ai_protocol = 0;
#endif
		hints.ai_flags = AI_PASSIVE;

		addrinfo *info;

		if (getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &info) != 0) {
			last_error = "getaddrinfo failed";
			free();
			return false;
		}

		TCP_SOCKET sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

#ifdef _WIN32
		if (sock == INVALID_SOCKET) {
			last_error = WSAGetLastError();
#else
		if (sock == -1) {
			last_error = strerror(errno);
#endif
			freeaddrinfo(info);
			return false;
		}

#ifdef _WIN32
		if (bind(sock, info->ai_addr, static_cast<int>(info->ai_addrlen)) == SOCKET_ERROR) {
			last_error = WSAGetLastError();
#else
		if (bind(sock, info->ai_addr, info->ai_addrlen) != 0) {
			last_error = strerror(errno);
#endif
			freeaddrinfo(info);
			close(sock);
			return false;
		}

		freeaddrinfo(info);

		_sock = sock;

		return true;
	}
	
	bool listen(TCP_SOCKET sock) {
#ifdef _WIN32
		if (::listen(sock, SOMAXCONN) == SOCKET_ERROR) {
			last_error = WSAGetLastError();
#else
		if (::listen(sock, SOMAXCONN) == -1) {
			last_error = strerror(errno);
#endif
			close(sock);
			return false;
		}

		return true;
	}

	bool accept(TCP_SOCKET sock, TCP_SOCKET &_client_sock) {
		TCP_SOCKET client_sock = ::accept(sock, nullptr, nullptr);

#ifdef _WIN32
		if (client_sock == INVALID_SOCKET) {
			last_error = WSAGetLastError();
#else
		if (client_sock == -1) {
			last_error = strerror(errno);
#endif
			return false;
		}

		_client_sock = client_sock;
		return true;
	}

	bool send(TCP_SOCKET sock, const char *buffer, int buffer_len) {
#ifdef _WIN32
		if (::send(sock, buffer, buffer_len, 0) == SOCKET_ERROR) {
			last_error = WSAGetLastError();
#else
		if (write(sock, buffer, buffer_len) == -1) {
			last_error = strerror(errno);
#endif
			return false;
		}

		return true;
	}

	int receive(TCP_SOCKET sock, char *buffer, int buffer_len) {
#ifdef _WIN32
		int num_bytes = recv(sock, buffer, buffer_len, 0);
#else
		int num_bytes = read(sock, buffer, buffer_len);
#endif

		if (num_bytes < 0) {
#ifdef _WIN32
			last_error = WSAGetLastError();
#else
			last_error = strerror(errno);
#endif
		}

		return num_bytes;
	}
}
