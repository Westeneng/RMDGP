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
 * File:   testCUdpMulticastSender.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 12, 2021, 3:29 PM
 */

#ifndef TESTCUDPMULTICASTSENDER_H
#define TESTCUDPMULTICASTSENDER_H

#include <cppunit/extensions/HelperMacros.h>
#include <netinet/in.h>

class testCUdpMulticastSender : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testCUdpMulticastSender);

    CPPUNIT_TEST(testSend);
    CPPUNIT_TEST(testSendThrow);
    CPPUNIT_TEST(testSendInterrupted);
    CPPUNIT_TEST(testSendWouldBlock);
    CPPUNIT_TEST(testOpen);
    CPPUNIT_TEST(testOpenThrow);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testDestructor);

    CPPUNIT_TEST_SUITE_END();

public:
    testCUdpMulticastSender();
    virtual ~testCUdpMulticastSender();
    void setUp();
    void tearDown();

private:
    void testConstructor();
    void testDestructor();
    void testOpen();
    void testOpenThrow();
    void testSend();
    void tstSendDataDriven(const std::string testName,
            const in_addr multicastAddress,
            const in_addr interfaceAddress, const int interfacePort,
            const in_addr otherInterfaceAddress);
    void testSendThrow();
    void testSendInterrupted();
    void testSendWouldBlock();
};

#endif /* TESTCUDPMULTICASTSENDER_H */

