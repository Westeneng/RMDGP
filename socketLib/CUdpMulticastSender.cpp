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
 * File:   CUdpMulticastSender.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 * 
 * Created on March 18, 2021, 5:08 PM
 */

#include "CSocketAddress.h"
#include "CUdpMulticastSender.h"
#include "CSocketProxy.h"        // includes sys/types.h and sys/socket.h
#include <arpa/inet.h>
#include <string.h>
#include <exception>
#include <sstream>

CUdpMulticastSender::CUdpMulticastSender() : multicastDestination(new sockaddr_in)
{
}

CUdpMulticastSender::~CUdpMulticastSender()
{
}

void CUdpMulticastSender::open(const std::string multicastAddress, int multicastPort,
         const std::string interfaceAddress, int localPort)
{
   const in_addr mcAddress = { inet_addr(multicastAddress.c_str()) };
   const in_addr ifAddress = { inet_addr(interfaceAddress.c_str()) };

   open(mcAddress, multicastPort, ifAddress, localPort);
}

void CUdpMulticastSender::open(const in_addr& multicastAddress, int multicastPort,
         const in_addr& interfaceAddress, int localPort)
{
   // Create a datagram socket.
   openUdpSocket();

   bind(interfaceAddress, localPort);

   *multicastDestination = {0};
   multicastDestination->sin_family = AF_INET;
   multicastDestination->sin_addr = multicastAddress;
   multicastDestination->sin_port = htons(multicastPort);

   if (proxy->setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, 
                        &interfaceAddress, sizeof(interfaceAddress)))
   {
      closeAndThrowRuntimeException("Error setting local interface");
   }
}

size_t CUdpMulticastSender::send(const void *buffer, size_t bufferSize)
{
   ssize_t result;

   do
   {
      result = proxy->sendto(fd, buffer, bufferSize, 0, 
                            (struct sockaddr*)multicastDestination.get(), sizeof(sockaddr_in));
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

in_addr CUdpMulticastSender::getMulticastIpAddress() const
{
   return multicastDestination->sin_addr;
}

int CUdpMulticastSender::getMulticastPort() const
{
   return ntohs(multicastDestination->sin_port);
}
