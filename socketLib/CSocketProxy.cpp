/*
 * Copyright (C) 2021 G.J. Westeneng (Gerald Westeneng)
 *
 * This file is part of RMDGP. Reliable Multicast DataGram Protocol
 *
 * RMDGP is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * RMDGP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RMDGP.   If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * File:   CSocketProxy.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on February 13, 2021, 12:01 PM
 */


#include "CSocketProxy.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ifaddrs.h>

CSocketProxy::CSocketProxy()
{
}

CSocketProxy::~CSocketProxy()
{
}

int CSocketProxy::socket(int socket_family, int socket_type, int protocol)
{
   return ::socket(socket_family, socket_type, protocol);
}

int CSocketProxy::bind(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
   return ::bind(fd, addr, addrlen);
}

int CSocketProxy::close(int fd)
{
   return ::close(fd);
}

int CSocketProxy::fcntl(int fd, int cmd, int param)
{
   return ::fcntl(fd, cmd, param);
}

ssize_t CSocketProxy::recvfrom(int fd, void *buf, size_t len, int flags,
                           struct sockaddr *src_addr, socklen_t *addrlen)
{
   return ::recvfrom(fd, buf, len, flags, src_addr, addrlen);
}

ssize_t CSocketProxy::sendto(int fd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen)
{
   return ::sendto(fd, buf, len, flags, dest_addr, addrlen);
}

int CSocketProxy::setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
   return ::setsockopt(fd, level, optname, optval, optlen);
}

int CSocketProxy::getErrno()
{
   return errno;
}

int CSocketProxy::getifaddrs(struct ifaddrs **ifap)
{
   return ::getifaddrs(ifap);
}

void CSocketProxy::freeifaddrs(struct ifaddrs *ifa)
{
   ::freeifaddrs(ifa);
}
