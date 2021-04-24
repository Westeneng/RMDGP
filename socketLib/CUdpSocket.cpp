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
 * File:   CUdpSocket.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 23, 2021, 2:00 PM
 */

#include "CUdpSocket.h"
#include "CSocketProxy.h"        // includes sys/types.h and sys/socket.h
#include <fcntl.h>
#include <sstream>
#include <string.h>


CUdpSocket::CUdpSocket() : sfd(-1), proxy(new CSocketProxy())
{
}

CUdpSocket::CUdpSocket(std::shared_ptr<CSocketProxy> sockProxy) : sfd(-1), proxy(sockProxy)
{
}

CUdpSocket::~CUdpSocket()
{
   if(sfd >= 0) proxy->close(sfd);
   sfd = -1;
}

void CUdpSocket::setNonBlocking() const
{
   int result = proxy->fcntl(sfd, F_GETFL, 0);
   if(result != -1)
   {
      int flags = result;
      result = proxy->fcntl(sfd, F_SETFL, flags | O_NONBLOCK);
   }
   if(result == -1)
   {
      int errorNbr = proxy->getErrno();
      std::ostringstream message;
      message << "Error setNonBlocking " << errorNbr << ": " << strerror(errorNbr);
      throw std::runtime_error(message.str());
   }
}

void CUdpSocket::openUdpSocket()
{
   if(sfd >= 0)
   {
      closeUdpSocket();
   }

   // Create a datagram socket
   sfd = proxy->socket(AF_INET, SOCK_DGRAM, 0);
   if(sfd < 0)
   {
      int errorNbr = proxy->getErrno();
      std::ostringstream message;
      message << "Error opening datagram socket " << errorNbr << ": " << strerror(errorNbr);
      throw std::runtime_error(message.str());
   }
}

void CUdpSocket::closeUdpSocket()
{
   if(proxy->close(sfd))
   {
      int errorNbr = proxy->getErrno();
      std::ostringstream message;
      message << "Error closing socket " << errorNbr << ": " << strerror(errorNbr);
      throw std::runtime_error(message.str());
   }
   sfd = -1;
}

bool CUdpSocket::isOpen() const
{
   return sfd >= 0;
}

void CUdpSocket::setSocketProxy(std::shared_ptr<CSocketProxy> sockProxy)
{
   proxy = sockProxy;
}
