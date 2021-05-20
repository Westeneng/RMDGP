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
 * File:   testCInterfaces.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 20, 2021, 11:06 AM
 */

#include "testCInterfaces.h"
#include "CSocketTestProxy.h"
#include "../CInterfaces.h"
#include <netinet/in.h>
#include <arpa/inet.h>

CPPUNIT_TEST_SUITE_REGISTRATION(testCInterfaces);

testCInterfaces::testCInterfaces()
{
}

testCInterfaces::~testCInterfaces()
{
}

void testCInterfaces::setUp()
{
}

void testCInterfaces::tearDown()
{
}

void testCInterfaces::testRetrieveInterfaceAdressFromAddress()
{
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);
   CInterfaces Interfaces(testProxy);
   const in_addr localAddress = { inet_addr("127.0.0.1") };
   const in_addr illegalAddress = { inet_addr("120.0.0.1") };
   const in_addr anyAddress = { INADDR_ANY };

   in_addr result = Interfaces.retrieveInterfaceAdressFromAddress(localAddress);
   CPPUNIT_ASSERT_EQUAL(localAddress.s_addr, result.s_addr);
   CPPUNIT_ASSERT_EQUAL(1, testProxy->getifaddrsCnt);
   CPPUNIT_ASSERT_EQUAL(1, testProxy->freeifaddrsCnt);

   result = Interfaces.retrieveInterfaceAdressFromAddress(illegalAddress);
   CPPUNIT_ASSERT_EQUAL(anyAddress.s_addr, result.s_addr);
   CPPUNIT_ASSERT_EQUAL(2, testProxy->getifaddrsCnt);
   CPPUNIT_ASSERT_EQUAL(2, testProxy->freeifaddrsCnt);
}

void testCInterfaces::testRetrieveInterfaceAdressFromAddressThrow()
{
   const in_addr localAddress = { inet_addr("127.0.0.1") };

   // create a test proxy to simulate an error on call of getifaddrs
   class CTestProxyGetifaddrsError : public CSocketTestProxy
   {
   public:
      virtual int getifaddrs(struct ifaddrs **ifap) override
      {
         getifaddrsCnt++; Errno = ENOMEM;
         return -1;
      }
   };

   std::shared_ptr<CTestProxyGetifaddrsError> testProxy( new CTestProxyGetifaddrsError );
   CInterfaces interfaces(testProxy);

   try
   {
      in_addr result = interfaces.retrieveInterfaceAdressFromAddress(localAddress);
      CPPUNIT_FAIL("Error, we expect retrieveInterfaceAdressFromAddress() to throw a runtime_error");
   }
   catch (std::runtime_error &re)
   {
      testProxy->verifyErrnoInMessage(re.what());
      CPPUNIT_ASSERT_EQUAL(1, testProxy->getifaddrsCnt);
      // verify that freeifaddrs is not called while getifaddrs failed
      CPPUNIT_ASSERT_EQUAL(0, testProxy->freeifaddrsCnt);
   }
}

void testCInterfaces::testGetIpV4InterfaceAddresses()
{
   // we expect that there are at least two interfaces. Loopback and another.
   std::set<in_addr_t> addresses = CInterfaces().getIpV4InterfaceAddresses();
   CPPUNIT_ASSERT_GREATER( (long unsigned int)(1), addresses.size() );

   // No address should be returned when any address is given
   const in_addr anyAddress = { inet_addr("0.0.0.0") };
   const in_addr mask = { inet_addr("255.255.255.255") };
   addresses = CInterfaces().getIpV4InterfaceAddresses(anyAddress, mask);
   CPPUNIT_ASSERT_EQUAL( (long unsigned int)(0), addresses.size() );
}

void testCInterfaces::testGetIpV4InterfaceAddressesLocal()
{
   const in_addr localAddress = { inet_addr("127.0.0.0") };
   const in_addr mask = { inet_addr("255.0.0.0") };

   std::set<in_addr_t> addresses = CInterfaces().getIpV4InterfaceAddresses(localAddress, mask);
   CPPUNIT_ASSERT_EQUAL( (long unsigned int)(1), addresses.size() );
   CPPUNIT_ASSERT_EQUAL( htonl(INADDR_LOOPBACK), *(addresses.begin()) );
}

void testCInterfaces::testGetIpV4InterfaceAddressesPrivate()
{
   const in_addr privateAddress = { inet_addr("192.168.0.0") };
   const in_addr mask = { inet_addr("255.255.0.0") };

   std::set<in_addr_t> addresses = CInterfaces().getIpV4InterfaceAddresses(privateAddress, mask);
   if(addresses.empty())
      std::cout << "No 192.168 range interfaces found ";
   else
   {
      in_addr address = { *(addresses.begin()) };
      std::cout << "Found:" << inet_ntoa(address) << " ";
   }
}

void testCInterfaces::testGetAPrivateNetworkInterface()
{
   // we expect there is at least one private network interface!
   const in_addr_t privateInterfaceAddress = CInterfaces().getAPrivateNetworkInterface();

   CPPUNIT_ASSERT(INADDR_ANY != privateInterfaceAddress);

   in_addr address = { privateInterfaceAddress };
   std::cout << "Found:" << inet_ntoa(address) << " ";
}
