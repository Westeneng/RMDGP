/*
 * Copyright (C) 2021 G.J. Westeneng (Gerald Westeneng)
 *
 * This file is part of RMDGP. Reliable Multicast DataGram Protocol
 *
 * RMDGP is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 *
 * RMDGP is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with RMDGP.
 *   If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * File:   CSocket.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on February 13, 2021, 12:01 PM
 */

#ifndef CSOCKET_PROXY_H
#define CSOCKET_PROXY_H

/**
 * @class CSocketProxy
 *   This class is just a thin layer around the socket related system calls for test purpose
 *   The caller is responsible for correct calling of the system calls. No error handling added.
 */

#include <sys/types.h>
#include <sys/socket.h>

class CSocketProxy {
public:
    CSocketProxy();
    CSocketProxy(const CSocketProxy& ) = delete;
    CSocketProxy &operator=(const CSocketProxy& ) = delete;
    virtual ~CSocketProxy();

    virtual int bind(int fd, const struct sockaddr *addr, socklen_t addrlen);
    virtual int close(int fd);
    virtual int fcntl(int fd, int cmd, int param);
    virtual ssize_t recvfrom(int fd, void *buf, size_t len, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen);
    ssize_t sendto(int fd, const void *buf, size_t len, int flags,
                    const struct sockaddr *dest_addr, socklen_t addrlen);
    virtual int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
    virtual int socket(int socket_family, int socket_type, int protocol);

    virtual int getifaddrs(struct ifaddrs **ifap);
    virtual void freeifaddrs(struct ifaddrs *ifa);

    virtual int getErrno();
};

#endif /* CSOCKET_PROXY_H */

