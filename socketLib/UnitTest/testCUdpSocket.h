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
 * File:   testCUdpSocket.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 23, 2021, 3:13 PM
 */

#ifndef TESTCUDPSOCKET_H
#define TESTCUDPSOCKET_H

#include <cppunit/extensions/HelperMacros.h>
class CUdpSocket;
struct in_addr;

class testCUdpSocket : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testCUdpSocket);

    CPPUNIT_TEST(testSetNonBlocking);
    CPPUNIT_TEST(testCloseUdpSocket);
    CPPUNIT_TEST(testOpenUdpSocket);
    CPPUNIT_TEST(testRetrieveInterfaceAdressFromAddress);
    CPPUNIT_TEST(testRetrieveInterfaceAdressFromAddressThrow);
    CPPUNIT_TEST(testCloseAndThrowRuntimeException);
    CPPUNIT_TEST(testBind);
    CPPUNIT_TEST(testBindException);
    CPPUNIT_TEST(testSendTo);
    CPPUNIT_TEST(testSendToThrow);
    CPPUNIT_TEST(testSendToInterrupted);
    CPPUNIT_TEST(testSendToWouldBlock);
//    CPPUNIT_TEST(testReceiveFrom); already tested at testSendTo
    CPPUNIT_TEST(testReceiveFromThrow);
    CPPUNIT_TEST(testReceiveFromInterrupted);
    CPPUNIT_TEST(testReceiveFromWouldBlock);

    CPPUNIT_TEST(testGetLocalSockAddress);

    CPPUNIT_TEST_SUITE_END();

public:
    testCUdpSocket();
    virtual ~testCUdpSocket();
    void setUp();
    void tearDown();

private:
    void testSetNonBlocking();
    void testCloseUdpSocket();
    void testOpenUdpSocket();
    void testRetrieveInterfaceAdressFromAddress();
    void testRetrieveInterfaceAdressFromAddressThrow();
    void testCloseAndThrowRuntimeException();
    void testBind();
    void tstBindDataDriven( const std::string testName,
                            const struct in_addr &interfaceAddress,
                            int port,
                            bool equalPortExpected);
    void testBindException();
    void testSendTo();
    void testSendToThrow();
    void testSendToInterrupted();
    void testSendToWouldBlock();
    void testReceiveFromThrow();
    void testReceiveFromInterrupted();
    void testReceiveFromWouldBlock();
    void testGetLocalSockAddress();
};

#endif /* TESTCUDPSOCKET_H */

