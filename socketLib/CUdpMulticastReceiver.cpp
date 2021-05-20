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
 * File:   CUdpMulticastReceiver.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 * 
 * Created on February 13, 2021, 9:25 PM
 */

#include "CUdpMulticastReceiver.h"
#include "CSocketProxy.h"        // includes sys/types.h and sys/socket.h
#include "CInterfaces.h"
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <ifaddrs.h>
#include <string.h>
#include <exception>
#include <sstream>
#include <iostream>


CUdpMulticastReceiver::CUdpMulticastReceiver() : sourceIpAddress(new in_addr), sourcePortNumber(0), 
                                          multicastIpAddress(new in_addr), multicastPortNumber(0)
{
   sourceIpAddress->s_addr = 0;
   multicastIpAddress->s_addr = 0;
}

CUdpMulticastReceiver::CUdpMulticastReceiver(const CUdpMulticastReceiver& orig) :
               CUdpSocket(orig.proxy), sourceIpAddress(new in_addr), sourcePortNumber(0), 
               multicastIpAddress(new in_addr), multicastPortNumber(0)
{
   sourceIpAddress->s_addr = 0;
   multicastIpAddress->s_addr = 0;

   if(orig.isOpen())
   {
      open(*orig.multicastIpAddress, orig.multicastPortNumber, 
              *orig.sourceIpAddress, orig.sourcePortNumber);
   }
   else
   {
      *multicastIpAddress = *orig.multicastIpAddress;
      *sourceIpAddress = *orig.sourceIpAddress;
      multicastPortNumber = orig.multicastPortNumber;
      sourcePortNumber = orig.sourcePortNumber;
   }
}

CUdpMulticastReceiver::~CUdpMulticastReceiver()
{
}

void CUdpMulticastReceiver::open(const std::string multicastAddress, int multicastPort,
         const std::string sourceAddress, int sourcePort)
{
   const in_addr mcAddress = { inet_addr(multicastAddress.c_str()) };
   const in_addr srcAddress = { inet_addr(sourceAddress.c_str()) };

   open(mcAddress, multicastPort, srcAddress, sourcePort);
}

void CUdpMulticastReceiver::open(const in_addr &multicastAddress, int multicastPort,
               const in_addr &sourceAddress, int sourcePort)
{
   sourceIpAddress->s_addr = sourceAddress.s_addr;
   sourcePortNumber = sourcePort;
   multicastIpAddress->s_addr = multicastAddress.s_addr;
   multicastPortNumber = multicastPort;

   // Create a datagram socket on which to receive.
   openUdpSocket();

   // Enable SO_REUSEADDR to allow others to receive copies of the datagrams
   int reuse=1;

   if(proxy->setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)))
   {
      closeAndThrowRuntimeException("Error setting SO_REUSEADDR");
   }

   // Bind the socket to local interface and port.
   // !!! be aware that for UDP multicast we MUST bind to INADDR_ANY.
   // For instance, if we bind to loopback and also join at loopback we will not receive any
   // multicast message.
   const in_addr anyAddress = { INADDR_ANY };
   bind(anyAddress, multicastPort);

   std::set<in_addr_t> interfaces;
   // if source Ip is set to INADDR_ANY, then join to all IPv4 interfaces.
   if(sourceIpAddress->s_addr == INADDR_ANY)
   {
      interfaces = CInterfaces(proxy).getIpV4InterfaceAddresses();
   }
   else
   {
      in_addr interfaceAddress = 
                        CInterfaces(proxy).retrieveInterfaceAdressFromAddress(*sourceIpAddress);
      interfaces.insert(interfaceAddress.s_addr);
   }

   for(auto it = interfaces.begin(); it != interfaces.end(); ++it)
   {
      // Join the multicast group
      struct ip_mreq_source mcGroup = { 0 };
      mcGroup.imr_multiaddr.s_addr = multicastAddress.s_addr;
      mcGroup.imr_sourceaddr.s_addr = sourceAddress.s_addr;
      mcGroup.imr_interface.s_addr = *it;

      if(proxy->setsockopt(fd, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (char *)&mcGroup, sizeof(mcGroup)))
      {
         closeAndThrowRuntimeException("Error joining multicast group");
      }
   }

   // set receive multicast all to 0. But I am not sure this is correct. 
   // I couldn't discover different behavior. Probably this doesn't provides something useful!
   int rcv_multicast_all = 0;   
   if(proxy->setsockopt(fd, SOL_SOCKET, IP_MULTICAST_ALL,
                       (char *)&rcv_multicast_all, sizeof(rcv_multicast_all)) )
   {
      closeAndThrowRuntimeException("Error setting IP_MULTICAST_ALL");
   }
}

size_t CUdpMulticastReceiver::receive(void *buffer, size_t bufferSize)
{
   ssize_t result;
   struct sockaddr_in srcAddress;
   socklen_t adressLen = sizeof(srcAddress);

   // receive until an error is reported except EINTR
   // or a message is received from expected source address
   // and from the expected source port number when the port number is set.
   do
   {
      do
      {
         do
         {
            result = proxy->recvfrom(fd, buffer, bufferSize, 0, 
                                    (struct sockaddr *)&srcAddress, &adressLen);
         } while(result==-1 && proxy->getErrno() == EINTR);
      } while((result!=-1) && sourceIpAddress->s_addr &&
              (sourceIpAddress->s_addr != srcAddress.sin_addr.s_addr));
   } while((result!=-1) && (sourcePortNumber>0) && (htons(sourcePortNumber) != srcAddress.sin_port));

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
         message << "Error recvfrom " << errorNbr << ": " << strerror(errorNbr);
         throw std::runtime_error(message.str());
      }
   }
   else if(sourcePortNumber==-1)
   {
      sourcePortNumber = ntohs(srcAddress.sin_port);
   }
   
   return result;
}

int CUdpMulticastReceiver::getSourcePortNumber() const
{
   return sourcePortNumber;
}

in_addr CUdpMulticastReceiver::getSourceIpAddress() const
{
   return *sourceIpAddress;
}
        
in_addr CUdpMulticastReceiver::getMulticastIpAddress() const
{
   return *multicastIpAddress;
}

int CUdpMulticastReceiver::getMulticastPort() const
{
   return multicastPortNumber;
}
