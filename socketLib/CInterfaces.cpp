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
 * File:   CInterfaces.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 20, 2021, 10:32 AM
 */

#include "CInterfaces.h"
#include "CSocketProxy.h"        // includes sys/types.h and sys/socket.h
#include <sstream>
#include <string.h>
#include <netinet/in.h>
#include <ifaddrs.h>
//#include <netinet/ip.h>
#include <arpa/inet.h>

///
namespace
{

class CScopedIfaddr {   // frees ifaddr when object goes out of scope
public:
   CScopedIfaddr(std::shared_ptr<CSocketProxy> Proxy) : proxy(Proxy), ifaddr(NULL)
   {
      if (proxy->getifaddrs(&ifaddr) == -1)
      {
         int errorNbr = proxy->getErrno();
         std::ostringstream message;

         message << "Error getifaddrs " << errorNbr << ": " << strerror(errorNbr);
         throw std::runtime_error(message.str());
      }
   }
   ~CScopedIfaddr() { if(ifaddr!=NULL) proxy->freeifaddrs(ifaddr); }

   struct ifaddrs *ifaddr;
   std::shared_ptr<CSocketProxy> proxy;
};

}  // namespace


CInterfaces::CInterfaces() : proxy(CSocketProxySingleton::get())
{
}

CInterfaces::CInterfaces(std::shared_ptr<CSocketProxy> Proxy) : proxy(Proxy)
{
}

CInterfaces::~CInterfaces()
{
}

in_addr CInterfaces::retrieveInterfaceAdressFromAddress(const in_addr address)
{
   CScopedIfaddr scoped(this->proxy);

   // by default the address is set to INADDR_ANY.
   in_addr result = { INADDR_ANY };

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

std::set<in_addr_t> CInterfaces::getIpV4InterfaceAddresses(const in_addr_t address,
                                                           const in_addr_t mask)
{
   CScopedIfaddr scoped(this->proxy);
   std::set<in_addr_t> addresses;

   for (struct ifaddrs *ifa = scoped.ifaddr; ifa != NULL; ifa = ifa->ifa_next)
   {
      if (ifa->ifa_addr == NULL)
          continue;
      switch(ifa->ifa_addr->sa_family)
      {
         case AF_INET:
         {
            in_addr_t ifAddress = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;

            if((ifAddress & mask) == (address & mask))
            {
               addresses.insert(ifAddress);
            }
            break;
         }
      }
   }

   return addresses;
}

std::set<in_addr_t> CInterfaces::getIpV4InterfaceAddresses()
{
   const in_addr anyAddress = { 0 };

   return getIpV4InterfaceAddresses(anyAddress, anyAddress);
}

in_addr_t CInterfaces::getAPrivateNetworkInterface()
{
   const in_addr classCAddress = { inet_addr("192.168.0.0") };
   const in_addr classCMask = { inet_addr("255.255.0.0") };
   std::set<in_addr_t> addresses = getIpV4InterfaceAddresses(classCAddress, classCMask);
   if(addresses.size()>0)
      return *(addresses.begin());

   const in_addr classBAddress = { inet_addr("172.16.0.0") };
   const in_addr classBMask = { inet_addr("255.240.0.0") };
   addresses = getIpV4InterfaceAddresses(classBAddress, classBMask);
   if(addresses.size()>0)
      return *(addresses.begin());

   const in_addr classAAddress = { inet_addr("10.0.0.0") };
   const in_addr classAMask = { inet_addr("255.0.0.0") };
   addresses = getIpV4InterfaceAddresses(classAAddress, classAMask);
   if(addresses.size()>0)
      return *(addresses.begin());

   return INADDR_ANY;
}
