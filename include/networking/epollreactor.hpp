#pragma once

#include <sys/epoll.h>

namespace networking {

class EpollReactor {
public:
    EpollReactor() {
        int rc = epoll_create1
    }

private:
    int epoll_fd { -1 };
};

}