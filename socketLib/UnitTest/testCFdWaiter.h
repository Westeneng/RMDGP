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
 * File:   testCFdWaiter.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 1, 2021, 9:20 AM
 */

#ifndef TESTCFDWAITER_H
#define TESTCFDWAITER_H

#include <cppunit/extensions/HelperMacros.h>

class testCFdWaiter : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testCFdWaiter);

    CPPUNIT_TEST(testAddReadFileDescriptor);
    CPPUNIT_TEST(testDelReadFileDescriptor);
    CPPUNIT_TEST(testAddWriteFileDescriptor);
    CPPUNIT_TEST(testDelWriteFileDescriptor);
    CPPUNIT_TEST(testWaitUntil);
    CPPUNIT_TEST(testWaitUntilInterrupted);
    CPPUNIT_TEST(testWaitUntilThrows);

    CPPUNIT_TEST_SUITE_END();

public:
    testCFdWaiter();
    virtual ~testCFdWaiter();
    void setUp();
    void tearDown();

private:
    void testAddReadFileDescriptor();
    void testDelReadFileDescriptor();
    void testAddWriteFileDescriptor();
    void testDelWriteFileDescriptor();
    void testWaitUntil();
    void testWaitUntilInterrupted();
    void testWaitUntilThrows();
};

#endif /* TESTCCLOCK_H */

