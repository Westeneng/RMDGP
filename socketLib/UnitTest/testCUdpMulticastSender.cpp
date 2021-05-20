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
 * File:   testCUdpMulticastSender.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 * 
 * Created on May 12, 2021, 3:29 PM
 */

#include "testCUdpMulticastSender.h"
#include "../CUdpMulticastSender.h"
#include "../CUdpMulticastReceiver.h"
#include "../CFdWaiter.h"
#include "../CClock.h"
#include "../CInterfaces.h"
#include "CSocketTestProxy.h"
#include <arpa/inet.h>

CPPUNIT_TEST_SUITE_REGISTRATION(testCUdpMulticastSender);

testCUdpMulticastSender::testCUdpMulticastSender()
{
}

testCUdpMulticastSender::~testCUdpMulticastSender()
{
}

void testCUdpMulticastSender::setUp()
{
}

void testCUdpMulticastSender::tearDown()
{
}

void testCUdpMulticastSender::testConstructor()
{
   CUdpMulticastSender UdpMulticastSender;
   const std::string emptyString;

   CPPUNIT_ASSERT_EQUAL(false, UdpMulticastSender.isOpen());
   CPPUNIT_ASSERT_EQUAL(0, UdpMulticastSender.getSenderPort());
}

void testCUdpMulticastSender::testDestructor()
{
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxy);

   // test that close is Not called
   {
      CUdpMulticastSender UdpMulticastSender;

      UdpMulticastSender.setSocketProxy(testProxy);
   }
   CPPUNIT_ASSERT_EQUAL(0, testProxy->closeCnt);

   // test that close is called
   {
      CUdpMulticastSender UdpMulticastSender;

      UdpMulticastSender.setSocketProxy(testProxy);

      CPPUNIT_ASSERT_NO_THROW(UdpMulticastSender.open("225.1.1.1", 7000, "127.0.0.1"));
   }
   CPPUNIT_ASSERT_EQUAL(1, testProxy->closeCnt);
}

void testCUdpMulticastSender::testOpen()
{
   {
      CUdpMulticastSender UdpMulticastSender;

      CPPUNIT_ASSERT_NO_THROW(UdpMulticastSender.open("225.1.1.1", 7000, "127.0.0.1"));

      CPPUNIT_ASSERT_EQUAL(true, UdpMulticastSender.isOpen());
      CPPUNIT_ASSERT_GREATER(100, UdpMulticastSender.getSenderPort());
   }
   {
      CUdpMulticastSender UdpMulticastSender;

      CPPUNIT_ASSERT_NO_THROW(UdpMulticastSender.open("225.1.1.1", 7000, "127.0.0.1", 7001));

      CPPUNIT_ASSERT_EQUAL(true, UdpMulticastSender.isOpen());
      CPPUNIT_ASSERT_EQUAL(7001, UdpMulticastSender.getSenderPort());
   }
}

void testCUdpMulticastSender::testOpenThrow()
{
   class CTestProxySetsockoptError : public CSocketTestProxy
   {
   public:
      virtual int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
         override
      {
         if(optname==IP_MULTICAST_IF)
         {
            Errno = ENOTSOCK; return -1;
         }
         return CSocketTestProxy::setsockopt(fd, level, optname, optval, optlen);
      }
   };

   CUdpMulticastSender UdpMulticastSender;

   // we expect an exception when illegal local address is given
   CPPUNIT_ASSERT_THROW(UdpMulticastSender.open("225.1.1.1", 7000, "1.0.0.1", 7001),
           std::runtime_error);
   // verify that socket is closed at exception
   CPPUNIT_ASSERT(UdpMulticastSender.isOpen()==false);

   // now test setsockopt error
   std::shared_ptr<CSocketTestProxy> testProxy(new CTestProxySetsockoptError);
   UdpMulticastSender.setSocketProxy(testProxy);
   
   try
   {
      UdpMulticastSender.open("225.1.1.1", 7000, "127.0.0.1", 7001);
      CPPUNIT_FAIL("Error, we expect open() to throw a runtime_error");
   }
   catch(std::runtime_error &re)
   {
      std::string message(re.what());
      std::string expectedText("Error setting local interface");

      if(message.find(expectedText) == std::string::npos)
      {
         std::string failMessage = "Msg '" + message + "' Doesn't contain '" +  expectedText + "'";

         CPPUNIT_FAIL(failMessage);
      }
      testProxy->verifyErrnoInMessage(message);
   }
   // verify that socket is closed at exception
   CPPUNIT_ASSERT(UdpMulticastSender.isOpen()==false);
   
}

void testCUdpMulticastSender::testSend()
{
   const in_addr interfaceAddress = { CInterfaces().getAPrivateNetworkInterface() };
   const in_addr multicastAddress = { inet_addr("225.1.1.1") };
   const in_addr localHost        = { inet_addr("127.0.0.1") };
   const std::string ifAddress = inet_ntoa(interfaceAddress);
   
   tstSendDataDriven("private network interface", multicastAddress, interfaceAddress, 0, localHost);
   tstSendDataDriven("loopback interface", multicastAddress, localHost, 0, interfaceAddress);
   tstSendDataDriven("private network interface, specific port",
                           multicastAddress, interfaceAddress, 7001, localHost);
   tstSendDataDriven("loopback interface, specific port",
                           multicastAddress, localHost, 7001, interfaceAddress);
}

void testCUdpMulticastSender::tstSendDataDriven(const std::string testName, 
                                       const in_addr multicastAddress,
                                       const in_addr interfaceAddress, const int interfacePort,
                                       const in_addr otherInterfaceAddress)
{
   CUdpMulticastSender UdpMulticastSender;
   CUdpMulticastReceiver receiver1, receiver2;

   CPPUNIT_ASSERT_NO_THROW_MESSAGE(testName,
                  UdpMulticastSender.open(multicastAddress, 7000, interfaceAddress, interfacePort));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE(testName,
                  receiver1.open(multicastAddress, 7000, interfaceAddress, interfacePort));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE(testName,
                  receiver2.open(multicastAddress, 7000, otherInterfaceAddress, interfacePort));
   
   CPPUNIT_ASSERT_NO_THROW_MESSAGE(testName, receiver1.setNonBlocking());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE(testName, receiver2.setNonBlocking());
   
   char testMessage[] = "Hi, this is a HAPPY test message :-)";
   CPPUNIT_ASSERT_EQUAL_MESSAGE(testName,
                  sizeof(testMessage), UdpMulticastSender.send(testMessage, sizeof(testMessage)));

   CFdWaiter fdWaiter;
   const CTime milliSec(0, CTime::nsecInMillisec);
   CTime currentTime;
   fdWaiter.addReadFileDescriptor(&receiver1);
   CPPUNIT_ASSERT_EQUAL(false, fdWaiter.waitUntil(CClock::getMonotonicTime(currentTime) + milliSec));
   {
      char buffer[128] = {0};
      CPPUNIT_ASSERT_EQUAL(sizeof(testMessage), receiver1.receive(buffer, sizeof(buffer)));
      CPPUNIT_ASSERT_EQUAL(std::string(testMessage), std::string(buffer));
   }
   fdWaiter.delReadFileDescriptor(&receiver1);
}
        
void testCUdpMulticastSender::testSendThrow()
{
   CUdpMulticastSender UdpMulticastSender;
   char testMessage[] = "Hi, this is a sad test message :-(";
   
   // throws because socket is not opened.
   CPPUNIT_ASSERT_THROW(UdpMulticastSender.send(testMessage, sizeof(testMessage)),
               std::runtime_error);
}

void testCUdpMulticastSender::testSendInterrupted()
{
   CUdpMulticastSender UdpMulticastSender;
   char testMessage[] = "Hi, this is a happy test message :-)";

   // define a test proxy to simulate that sendto system call reports Interrupted three times
   class CSocketTestProxySendToError : public CSocketTestProxy
   {
   public:
      virtual ssize_t sendto(int fd, const void *buf, size_t len, int flags,
                          const struct sockaddr *dest_addr, socklen_t addrlen) override
      {
         if(++sendtoCnt > 3)
         {
            Errno = 0;
            return CSocketProxy::sendto(fd, buf, len, flags, dest_addr, addrlen);
         }
         Errno = EINTR;
         return -1;
      }
   };
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxySendToError);
   UdpMulticastSender.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW(UdpMulticastSender.open("225.1.1.1", 7000, "127.0.0.1", 7001));
   CPPUNIT_ASSERT_EQUAL(sizeof(testMessage),
                        UdpMulticastSender.send(testMessage, sizeof(testMessage)));
   CPPUNIT_ASSERT_EQUAL(4, testProxy->sendtoCnt);
}

void testCUdpMulticastSender::testSendWouldBlock()
{
   CUdpMulticastSender UdpMulticastSender;
   char testMessage[] = "Hi, this is a test message";

   // define a test proxy to simulate that sendto system call reports EWOULDBLOCK
   class CSocketTestProxySendToError : public CSocketTestProxy
   {
   public:
      virtual ssize_t sendto(int fd, const void *buf, size_t len, int flags,
                          const struct sockaddr *dest_addr, socklen_t addrlen) override
      {
         Errno = (sendtoCnt%2 == 0) ? EWOULDBLOCK : EAGAIN;
         ++sendtoCnt; return -1;
      }
   };
   std::shared_ptr<CSocketTestProxy> testProxy(new CSocketTestProxySendToError);
   UdpMulticastSender.setSocketProxy(testProxy);

   CPPUNIT_ASSERT_NO_THROW(UdpMulticastSender.open("225.1.1.1", 7000, "127.0.0.1", 7001));
   CPPUNIT_ASSERT_EQUAL(size_t(0), UdpMulticastSender.send(testMessage, sizeof(testMessage)));
   CPPUNIT_ASSERT_EQUAL(1, testProxy->sendtoCnt);
   CPPUNIT_ASSERT_EQUAL(size_t(0), UdpMulticastSender.send(testMessage, sizeof(testMessage)));
   CPPUNIT_ASSERT_EQUAL(2, testProxy->sendtoCnt);
}

