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
 * File:   testCClock.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 1, 2021, 9:20 AM
 */

#include "testCClock.h"
#include "../CClock.h"
#include "../CTime.h"


CPPUNIT_TEST_SUITE_REGISTRATION(testCClock);


testCClock::testCClock()
{
}

testCClock::~testCClock()
{
}

void testCClock::setUp()
{
}

void testCClock::tearDown()
{
}

void testCClock::testGetMonotonicTime()
{
   CTime time;
   const CTime zeroTime;

   CPPUNIT_ASSERT_EQUAL(long(0), time.tv_nsec);
   CPPUNIT_ASSERT_EQUAL(time_t(0), time.tv_sec);
   CPPUNIT_ASSERT_EQUAL(zeroTime, time);

   CPPUNIT_ASSERT_NO_THROW(CClock::getMonotonicTime(time));

   CPPUNIT_ASSERT_GREATER(zeroTime, time);

   // currently not possible to test what happens when OS returns -1 on clock_gettime call
   // because we will get a Segmentation fault in advance.
//   CTime * const pTime = nullptr;
//   CPPUNIT_ASSERT_THROW(CClock::getMonotonicTime(*pTime), std::runtime_error);
}
