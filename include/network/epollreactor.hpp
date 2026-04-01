#pragma once

#include <fcntl.h>
#include <sys/epoll.h>
#include <stdexcept>
#include <unistd.h>
#include <utility>

#include "TCPSocket.hpp"

namespace network {

class EpollReactor {
public:
    EpollReactor() 
        : epoll_fd_ { epoll_create1(EPOLL_CLOEXEC) } {
        if (epoll_fd_ == -1) {
            throw std::runtime_error("Unable to create epoll instance.");
        }
    }

    ~EpollReactor() {
        if (epoll_fd_ != -1) {
            close(epoll_fd_);
        }
    }

    // disable copy semantics
    EpollReactor(const EpollReactor&) = delete;
    EpollReactor& operator=(const EpollReactor&) = delete;

    // allow move semantics
    EpollReactor(EpollReactor&& other)
        : epoll_fd_ { std::exchange(other.epoll_fd_, -1) }
    { }

    EpollReactor& operator=(EpollReactor&& other) {
        if (this == &other) {
            return *this;
        }

        if (epoll_fd_ != -1) {
            close(epoll_fd_);
        }

        epoll_fd_ = std::exchange(other.epoll_fd_, -1);
        return *this;
    }

    void add_socket(int fd) {
        if (fd == -1) [[unlikely]] { 
            throw std::runtime_error("File descriptor passed in is invalid.");
        }

        struct epoll_event event{};
        // edge triggered for receiving inputs
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = fd;

        int rc = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
        if (rc == -1) {
            throw std::runtime_error("Unable to add file descriptor");
        }
    }

    void run(TCPSocket& server_socket) {
        struct epoll_event events[MAX_EVENTS];
        int ready_count = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1); // -1 to block until an event occurs

        for (int i = 0; i < ready_count; ++i) {
            int fd = events[i].data.fd;
            // a new client is trying to connect, accept them if possible
            if (server_socket.get_fd() == fd) {
                while (true) {
                    int client_fd = server_socket.accept();
                    if (client_fd == -1) {
                        break;
                    }

                    std::cout << "Accepting client connection.\n";
                    
                    int client_fd_flags = fcntl(client_fd, F_GETFL, 0);
                    if (client_fd_flags == -1 || fcntl(client_fd, F_SETFL, client_fd_flags | O_NONBLOCK) == -1) {
                        std::cout << "Unable to set client file descriptor as non blocking.\n";
                        close(client_fd);
                        continue;
                    };
                    add_socket(client_fd);
                }
            } else {
                // a client socket woke up instead
                // Note: make sure to read all of the data as we are using edge triggered. See man page for details
                printf("Received from client: ");
                while (true) {
                    char buffer[MAX_BUFFER_SIZE];
                    int bytes_received = recv(fd, buffer, MAX_BUFFER_SIZE - 1, 0);
                    if (bytes_received == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            // no more data left in the buffer
                            break;
                        }
                        // unexpected error, close the client file descriptor
                        std::cout << "Unexpected error occurred.\n";
                        close(fd);
                        break;
                    } else if (bytes_received == 0) {
                        // client disconnected
                        std::cout << "Client has disconnected.\n";
                        close(fd);
                        break;
                    } else {
                        // make use of data, can be modified for actual use
                        buffer[bytes_received] = '\0';
                        printf("%s", buffer);
                    }
                }
            }
        }
    }

    int get_epollfd() { return epoll_fd_; }

private:
    int epoll_fd_ { -1 };
    static const int MAX_EVENTS { 64 };
    static const int MAX_BUFFER_SIZE { 1000 }; // should be tuned accordingly.

    using TCPSocket = network::TCPSocket;
};

}