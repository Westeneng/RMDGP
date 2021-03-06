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
#include "CSocketAddress.h"
#include <fcntl.h>
#include <sstream>
#include <string.h>
#include <netinet/ip.h>


CUdpSocket::CUdpSocket()
{
}

CUdpSocket::CUdpSocket(std::shared_ptr<CSocketProxy> sockProxy) : CFileDescriptor(sockProxy)
{
}

CUdpSocket::~CUdpSocket()
{
}

void CUdpSocket::setNonBlocking() const
{
   int result = proxy->fcntl(fd, F_GETFL, 0);
   if(result != -1)
   {
      int flags = result;
      result = proxy->fcntl(fd, F_SETFL, flags | O_NONBLOCK);
   }
   if(result == -1)
   {
      int errorNbr = proxy->getErrno();
      std::ostringstream message;
      message << "Error setNonBlocking " << errorNbr << ": " << strerror(errorNbr);
      throw std::runtime_error(message.str());
   }
}

void CUdpSocket::bind(const in_addr interfaceAddress, int port)
{
   // Bind the socket to local interface and port.
   struct sockaddr_in localSockAddress = { 0 };
   localSockAddress.sin_family = AF_INET;
   localSockAddress.sin_port = htons(port);
   localSockAddress.sin_addr = interfaceAddress;

   if(proxy->bind(fd, (struct sockaddr*)&localSockAddress, sizeof(localSockAddress)))
   {
      closeAndThrowRuntimeException("Error binding datagram socket");
   }
}

int CUdpSocket::getLocalPort() const
{
   return ntohs(getLocalSockAddress().sin_port);
}

in_addr CUdpSocket::getLocalAddress() const
{
   return getLocalSockAddress().sin_addr;
}

sockaddr_in CUdpSocket::getLocalSockAddress() const
{
   struct sockaddr_in sockAddress = { 0, { INADDR_ANY } };

   if(isOpen())
   {
      socklen_t adressLen = sizeof(sockAddress);

      if(getsockname(fd, (struct sockaddr *)&sockAddress, &adressLen))
      {
         int errorNbr = proxy->getErrno();
         std::ostringstream message;
         message << "Error retrieving sock address " << errorNbr << ": " << strerror(errorNbr);
         throw std::runtime_error(message.str());
      }
   }

   return sockAddress;
}

void CUdpSocket::openUdpSocket()
{
   if(fd >= 0)
   {
      closeUdpSocket();
   }

   // Create a datagram socket
   fd = proxy->socket(AF_INET, SOCK_DGRAM, 0);
   if(fd < 0)
   {
      int errorNbr = proxy->getErrno();
      std::ostringstream message;
      message << "Error opening datagram socket " << errorNbr << ": " << strerror(errorNbr);
      throw std::runtime_error(message.str());
   }
}

void CUdpSocket::closeUdpSocket()
{
   if(proxy->close(fd))
   {
      int errorNbr = proxy->getErrno();
      std::ostringstream message;
      message << "Error closing socket " << errorNbr << ": " << strerror(errorNbr);
      throw std::runtime_error(message.str());
   }
   fd = -1;
}

size_t CUdpSocket::sendTo(const void *buffer, size_t bufferSize, sockaddr_in *destination)
{
   ssize_t result;

   do
   {
      result = proxy->sendto(fd, buffer, bufferSize, 0,
                            (struct sockaddr*)destination, sizeof(sockaddr_in));
   } while(result==-1 && proxy->getErrno() == EINTR);

   if(result == -1)
   {
      int errorNbr = proxy->getErrno();
      if(errorNbr == EAGAIN || errorNbr == EWOULDBLOCK)
      {
         result = 0;
      }
      else
      {
         std::ostringstream message;
         message << "Error sendto " << errorNbr << ": " << strerror(errorNbr);
         throw std::runtime_error(message.str());
      }
   }

   return result;
}

size_t CUdpSocket::receiveFrom(void *buffer, size_t bufferSize, sockaddr_in *source)
{
   ssize_t result;
   socklen_t adressLen = sizeof(sockaddr_in);

   // receive until an error is reported except EINTR
   do
   {
      result = proxy->recvfrom(fd, buffer, bufferSize, 0, (struct sockaddr *)source, &adressLen);
   } while(result==-1 && proxy->getErrno() == EINTR);

   if(result == -1)
   {
      int errorNbr = proxy->getErrno();

      if(errorNbr == EAGAIN || errorNbr == EWOULDBLOCK)
      {
         result = 0;
      }
      else
      {
         std::ostringstream message;
         message << "Error receivefrom " << errorNbr << ": " << strerror(errorNbr);
         throw std::runtime_error(message.str());
      }
   }

   return result;
}

void CUdpSocket::closeAndThrowRuntimeException(const std::string matter)
{
   int errorNbr = proxy->getErrno();
   std::ostringstream message;

   message << matter << " " << errorNbr << ": " << strerror(errorNbr);
   try
   {
      closeUdpSocket();
   }
   catch(std::runtime_error &re)
   {
      message << " && " << re.what();
   }
   throw std::runtime_error(message.str());
}


