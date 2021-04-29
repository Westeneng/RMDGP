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
#include <netinet/ip.h>
#include <ifaddrs.h>


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

void CUdpSocket::bind(const in_addr interfaceAddress, int port)
{
   // Bind the socket to local interface and port.
   struct sockaddr_in localSockAddress = { 0 };
   localSockAddress.sin_family = AF_INET;
   localSockAddress.sin_port = htons(port);
   localSockAddress.sin_addr = interfaceAddress;

   if(proxy->bind(sfd, (struct sockaddr*)&localSockAddress, sizeof(localSockAddress)))
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

      if(getsockname(sfd, (struct sockaddr *)&sockAddress, &adressLen))
      {
         int errorNbr = proxy->getErrno();
         std::ostringstream message;
         message << "Error retrieving sender port " << errorNbr << ": " << strerror(errorNbr);
         throw std::runtime_error(message.str());
      }
   }

   return sockAddress;
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

in_addr CUdpSocket::retrieveInterfaceAdressFromAddress(const in_addr address)
{
   class CScopedIfaddr {   // frees ifaddr when object goes out of scope
   public:
      CScopedIfaddr(std::shared_ptr<CSocketProxy> Proxy) : proxy(Proxy), ifaddr(NULL) {}
      ~CScopedIfaddr() { if(ifaddr!=NULL) proxy->freeifaddrs(ifaddr); }

      struct ifaddrs *ifaddr;
      std::shared_ptr<CSocketProxy> proxy;
   } scoped(this->proxy);

   // by default the address is set to INADDR_ANY.
   in_addr result = { INADDR_ANY };

   if (proxy->getifaddrs(&scoped.ifaddr) == -1)
   {
      int errorNbr = proxy->getErrno();
      std::ostringstream message;

      message << "Error getifaddrs " << errorNbr << ": " << strerror(errorNbr);
      throw std::runtime_error(message.str());
   }
   for (struct ifaddrs *ifa = scoped.ifaddr; ifa != NULL; ifa = ifa->ifa_next)
   {
      if (ifa->ifa_addr == NULL)
          continue;
      switch(ifa->ifa_addr->sa_family)
      {
         case AF_INET:
         {
            in_addr_t ifAddress = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
            in_addr_t ifMask = ((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr.s_addr;

            if((ifAddress & ifMask) == (address.s_addr & ifMask))
            {
               result.s_addr = ifAddress;

               return result;
            }
            break;
         }
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


