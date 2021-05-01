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
 * File:   CClock.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on April 30, 2021, 8:03 PM
 */

#include "CClock.h"
#include "CTime.h"
#include <time.h>
#include <sstream>
#include <errno.h>
#include <string.h>

CClock::CClock()
{
}

CClock::~CClock()
{
}

CTime& CClock::getMonotonicTime(CTime &monotonicTime)
{
   if(clock_gettime(CLOCK_MONOTONIC, &monotonicTime))
   {
      std::ostringstream message;
      message << "Error clock_gettime CLOCK_MONOTONIC:" << errno << ": " << strerror(errno);
      throw std::runtime_error(message.str());
   }
   return monotonicTime;
}
