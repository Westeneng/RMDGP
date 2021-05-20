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
 * File:   testCUdpMulticastReceiver.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on Apr 14, 2021, 8:30:06 PM
 */

#include "testCUdpMulticastReceiver.h"
#include "CSocketTestProxy.h"
#include "../CUdpMulticastReceiver.h"
//#include "../CUdpMulticastSender.h"
#include <netinet/ip.h>
#include "../CSocketProxy.h"

CPPUNIT_TEST_SUITE_REGISTRATION(testCUdpMulticastReceiver);

testCUdpMulticastReceiver::testCUdpMulticastReceiver()
{
}

testCUdpMulticastReceiver::~testCUdpMulticastReceiver()
{
}

void testCUdpMulticastReceiver::setUp()
{
}

void testCUdpMulticastReceiver::tearDown()
{
}

void testCUdpMulticastReceiver::testConstructor()
{
   CUdpMulticastReceiver UdpMulticastReceiver;

   CPPUNIT_ASSERT_EQUAL(false, UdpMulticastReceiver.isOpen());
   CPPUNIT_ASSERT_EQUAL(in_addr_t(0), UdpMulticastReceiver.getMulticastIpAddress().s_addr);
   CPPUNIT_ASSERT_EQUAL(0, UdpMulticastReceiver.getMulticastPort());
   CPPUNIT_ASSERT_EQUAL(in_addr_t(0), UdpMulticastReceiver.getSourceIpAddress().s_addr);
   CPPUNIT_ASSERT_EQUAL(0, UdpMulticastReceiver.getSourcePortNumber());
}

void testCUdpMulticastReceiver::testCopyConstructor()
{
   CUdpMulticastReceiver orig;

   tstCopyConstructorDataDriven("Not opened MC receiver", orig);

   orig.open("225.1.1.1", 7000, "127.0.0.1", 1273);

   tstCopyConstructorDataDriven("Opened MC receiver", orig);
}

void testCUdpMulticastReceiver::tstCopyConstructorDataDriven(const std::string testName,
        const CUdpMulticastReceiver &orig)
{
   const CUdpMulticastReceiver copy(orig);

   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName, orig.isOpen(), copy.isOpen());
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName,
                        orig.getSourceIpAddress().s_addr, copy.getSourceIpAddress().s_addr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName, orig.getSourcePortNumber(), copy.getSourcePortNumber());
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName,
                        orig.getMulticastIpAddress().s_addr, copy.getMulticastIpAddress().s_addr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName, orig.getMulticastPort(), copy.getMulticastPort());
}

void testCUdpMulticastReceiver::testDestructor()
{
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);

   // test that close is Not called
   {
      CUdpMulticastReceiver udpMulticastReceiver;

      udpMulticastReceiver.setSocketProxy(testProxy);
   }
   CPPUNIT_ASSERT_EQUAL(0, testProxy->closeCnt);

   // test that close is called
   {
      CUdpMulticastReceiver udpMulticastReceiver;

      udpMulticastReceiver.setSocketProxy(testProxy);

      CPPUNIT_ASSERT_NO_THROW(udpMulticastReceiver.open("225.1.1.1", 7000, "127.0.0.1"));
   }
   CPPUNIT_ASSERT_EQUAL(1, testProxy->closeCnt);
}

void testCUdpMulticastReceiver::testOpen()
{
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);
   CUdpMulticastReceiver udpMulticastReceiver;

   udpMulticastReceiver.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW(udpMulticastReceiver.open("225.1.1.1", 7000, "127.0.0.1"));
   CPPUNIT_ASSERT(udpMulticastReceiver.isOpen());

   // verify that there is no resource leak regarding getifaddrs and freeifaddrs.
   CPPUNIT_ASSERT_EQUAL(1, testProxy->getifaddrsCnt);
   CPPUNIT_ASSERT_EQUAL(1, testProxy->freeifaddrsCnt);
   CPPUNIT_ASSERT_EQUAL(7000, udpMulticastReceiver.getLocalPort());
}

// namespace witch several socket test proxy's for the sake of testOpenThrow
namespace
{

class CTestProxySetsockoptError : public CSocketTestProxy
{
public:
   virtual int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
      override
   {
      Errno = ENOTSOCK; return -1;
   }
};

class CTestProxySetsockoptError2 : public CSocketTestProxy
{
public:
   virtual int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
      override
   {
      if(optname==IP_MULTICAST_ALL)
      {
         Errno = ENOTSOCK; return -1;
      }
      return CSocketTestProxy::setsockopt(fd, level, optname, optval, optlen);
   }
};

}

void testCUdpMulticastReceiver::testOpenThrow()
{
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);
   std::shared_ptr<CSocketTestProxy> testProxyReuseError(new CTestProxySetsockoptError);
   std::shared_ptr<CSocketTestProxy> testProxyMcAllError(new CTestProxySetsockoptError2);

   tstOpenThrowDataDriven("SO_REUSEADDR error",
            "225.1.1.1", 7000, "127.0.0.1", 0, "Error setting SO_REUSEADDR", ENOTSOCK,
            testProxyReuseError);
   tstOpenThrowDataDriven("invalid multicast address",
            "1.1.1.1", 7000, "127.0.0.1", 0, "Error joining multicast group", EINVAL,
            testProxy);
   tstOpenThrowDataDriven("IP_MULTICAST_ALL error",
            "225.1.1.1", 7000, "127.0.0.1", 0, "Error setting IP_MULTICAST_ALL", ENOTSOCK,
            testProxyMcAllError);
}

void testCUdpMulticastReceiver::tstOpenThrowDataDriven(const std::string testName,
                              const std::string multicastAddress, int multicastPort,
                              const std::string sourceAddress, int sourcePort,
                              const std::string expectedText, int expectedErrno,
                              std::shared_ptr<CSocketTestProxy> testProxy)
{
   CUdpMulticastReceiver udpMulticastReceiver;

   udpMulticastReceiver.setSocketProxy(testProxy);

   try
   {
      udpMulticastReceiver.open(multicastAddress, multicastPort, sourceAddress, sourcePort);
      CPPUNIT_FAIL(testName + ": Error, we expect open() to throw a runtime_error");
   }
   catch(std::runtime_error &re)
   {
      std::string message(re.what());

      if(message.find(expectedText) == std::string::npos)
      {
         std::string failMessage = "Msg '" + message + "' Doesn't contain '" +  expectedText + "'";

         CPPUNIT_FAIL(testName + ": " + failMessage);
      }

      testProxy->Errno = expectedErrno;
      testProxy->verifyErrnoInMessage(message, testName);
      testProxy->Errno = 0;

      CPPUNIT_ASSERT(udpMulticastReceiver.isOpen()==false);
   }
}

void testCUdpMulticastReceiver::testReceive()
{
   char buffer[256];
   char testMessage[] = "Hellooooo . . .";
   CUdpMulticastReceiver udpMulticastReceiver;
//   CUdpMulticastSender   udpMulticastSender;

   CPPUNIT_ASSERT_NO_THROW(udpMulticastReceiver.open("225.1.1.1", 7000, "127.0.0.1"));
   CPPUNIT_ASSERT_NO_THROW(udpMulticastReceiver.setNonBlocking());
//   CPPUNIT_ASSERT_NO_THROW(udpMulticastSender.open("225.1.1.1", 7000, "127.0.0.1"));

   CPPUNIT_FAIL("NOT IMPLEMENTED");
   // to test receive we need a multicast sender. TODO
//   CPPUNIT_ASSERT_NO_THROW(udpMulticastSender.send(testMessage, sizeof(testMessage)));
//   size_t result = udpMulticastReceiver.receive(buffer, sizeof(buffer));
//   CPPUNIT_ASSERT_EQUAL(sizeof(testMessage), result);
}

void testCUdpMulticastReceiver::testReceiveThrow()
{
   char buffer[256];
   CUdpMulticastReceiver udpMulticastReceiver;

   CPPUNIT_ASSERT_NO_THROW(udpMulticastReceiver.open("225.1.1.1", 7000, "127.0.0.1"));
   CPPUNIT_ASSERT_NO_THROW(udpMulticastReceiver.setNonBlocking());

   // define a test proxy to simulate that recvfrom system call fails
   class CSocketTestProxyRecvfromError : public CSocketTestProxy
   {
   public:
      virtual ssize_t recvfrom(int fd, void *buf, size_t len, int flags,
                      struct sockaddr *src_addr, socklen_t *addrlen) override
      {
         Errno = ENOMEM;   // simulate memory error
         recvfromCnt++; return -1;
      }
   };

   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxyRecvfromError);
   udpMulticastReceiver.setSocketProxy(testProxy);

   try
   {
      udpMulticastReceiver.receive(buffer, sizeof(buffer));
      CPPUNIT_FAIL("Error, we expect receive() to throw a runtime_error");
   }
   catch(std::runtime_error &re)
   {
      testProxy->verifyErrnoInMessage(re.what());
   }
}

void testCUdpMulticastReceiver::testGetSourcePortNumber()
{
   CUdpMulticastReceiver udpMulticastReceiver;

   CPPUNIT_ASSERT_EQUAL(0, udpMulticastReceiver.getSourcePortNumber());

   CPPUNIT_ASSERT_NO_THROW(udpMulticastReceiver.open("225.1.1.1", 7000, "127.0.0.1", 1273));

   CPPUNIT_ASSERT_EQUAL(1273, udpMulticastReceiver.getSourcePortNumber());
}

