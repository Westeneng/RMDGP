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
 * File:   CSocketTestProxy.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 23, 2021, 4:56 PM
 */

#ifndef CSOCKETTESTPROXY_H
#define CSOCKETTESTPROXY_H

#include <cppunit/extensions/HelperMacros.h>
#include "../CSocketProxy.h"

/// \brief a test helper class, to count system calls and to simulate system call behavior
class CSocketTestProxy : public CSocketProxy
{
public:
    CSocketTestProxy() : closeCnt(0), fcntlCnt(0), Errno(0), socketCnt(0), recvfromCnt(0),
        sendtoCnt(0), getifaddrsCnt(0), freeifaddrsCnt(0), setsockoptCnt(0), pselectCnt(0)  {}

    virtual int close(int fd) override
    {
        closeCnt++; return CSocketProxy::close(fd);
    }
    virtual int fcntl(int fd, int cmd, int param) override
    {
        fcntlCnt++; return CSocketProxy::fcntl(fd, cmd, param);
    }
    virtual int getErrno() override
    {
        return Errno ? Errno : CSocketProxy::getErrno();
    }
    virtual int socket(int socket_family, int socket_type, int protocol) override
    {
        socketCnt++; return CSocketProxy::socket(socket_family,socket_type, protocol);
    }
    virtual ssize_t recvfrom(int fd, void *buf, size_t len, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen) override
    {
        recvfromCnt++; return CSocketProxy::recvfrom(fd, buf, len, flags, src_addr, addrlen);
    }
    virtual ssize_t sendto(int fd, const void *buf, size_t len, int flags,
                        const struct sockaddr *dest_addr, socklen_t addrlen)
    {
        sendtoCnt++; return CSocketProxy::sendto(fd, buf, len, flags, dest_addr, addrlen);
    }
    virtual int getifaddrs(struct ifaddrs **ifap) override
    {
        getifaddrsCnt++; return CSocketProxy::getifaddrs(ifap);
    }
    virtual void freeifaddrs(struct ifaddrs *ifa) override
    {
        freeifaddrsCnt++; return CSocketProxy::freeifaddrs(ifa);
    }
    virtual int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
                    override
    {
        setsockoptCnt++; return CSocketProxy::setsockopt(fd, level, optname, optval, optlen);
    }
    virtual int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                        const struct timespec *timeout, const sigset_t *sigmask) override
    {
        pselectCnt++;
        return CSocketProxy::pselect(nfds, readfds, writefds, exceptfds, timeout, sigmask);
    }

    int setsockoptCnt;
    int closeCnt;
    int fcntlCnt;
    int socketCnt;
    int recvfromCnt;
    int sendtoCnt;
    int getifaddrsCnt;
    int freeifaddrsCnt;
    int pselectCnt;
    int Errno;

    //
    void verifyErrnoInMessage(const std::string &message, const std::string testName = "")
    {
      std::ostringstream errorNumberPart;
      errorNumberPart << " " << Errno << ":";

      // verify that expected error number part is in the message
      if(message.find(errorNumberPart.str()) == std::string::npos)
      {
         std::string failMessage = "Msg '" + message + "' Doesn't contain '" +
                 errorNumberPart.str() + "'";

         CPPUNIT_FAIL(testName + ": " + failMessage);
      }
    }
};

#endif /* CSOCKETTESTPROXY_H */

