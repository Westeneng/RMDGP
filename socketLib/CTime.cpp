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
 * File:   CTime.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 * 
 * Created on April 9, 2021, 8:27 AM
 */

#include "CTime.h"
#include <iostream>
#include <iomanip>

CTime::CTime()
{
   tv_sec = 0;
   tv_nsec = 0;  
}

CTime::CTime(time_t sec, long nsec)
{
   tv_sec = sec;
   tv_nsec = nsec;
   enforceNsecRange();
}

CTime::CTime(const CTime& orig)
{
   tv_sec = orig.tv_sec;
   tv_nsec = orig.tv_nsec;
}

CTime::~CTime()
{
}

CTime CTime::operator-(const CTime& other) const
{
   CTime result;
   
   result.tv_sec = tv_sec - other.tv_sec;
   result.tv_nsec = tv_nsec - other.tv_nsec;
   while(result.tv_nsec<0)
   {
      result.tv_nsec += nsecInSec;
      result.tv_sec--;
   }
   
   return result;
}

CTime& CTime::operator-=(const CTime& other)
{
   tv_sec -= other.tv_sec;
   tv_nsec -= other.tv_nsec;
   while(tv_nsec<0)
   {
      tv_nsec += nsecInSec;
      tv_sec--;
   }
   
   return *this;
}

CTime CTime::operator+(const CTime& other) const
{
   CTime result;
   
   result.tv_sec = tv_sec + other.tv_sec;
   result.tv_nsec = tv_nsec + other.tv_nsec;
   while(result.tv_nsec >= nsecInSec)
   {
      result.tv_nsec -= nsecInSec;
      result.tv_sec++;
   }
   
   return result;   
}

CTime CTime::operator*(int count) const
{
   CTime result;
   long long nanosecs = (long long)tv_nsec * (long long)count;
   
   result.tv_sec = tv_sec * count;
   if(nanosecs>=nsecInSec)
   {
      long long correctionSecs = nanosecs/nsecInSec;
      nanosecs -= correctionSecs * nsecInSec;
      result.tv_sec += correctionSecs;
   }
   else if(nanosecs<0)
   {
      long long correctionSecs = (-nanosecs + nsecInSec - 1)/nsecInSec;
      nanosecs += correctionSecs * nsecInSec;
      result.tv_sec -= correctionSecs;
   }
   result.tv_nsec = nanosecs;
   
   return result;
}

CTime& CTime::operator+=(const CTime& other)
{
   tv_sec += other.tv_sec;
   tv_nsec += other.tv_nsec;
   // an if would be sufficient to, but because the the class members are public, we make it a 
   // little more robust with a while
   while(tv_nsec >= nsecInSec)
   {
      tv_nsec -= nsecInSec;
      tv_sec++;
   }

   return *this;   
}

CTime& CTime::operator=(const CTime& other)
{
   tv_sec = other.tv_sec;
   tv_nsec = other.tv_nsec;
   
   return *this;
}

bool CTime::operator==(const CTime& other) const
{
   return ((tv_sec == other.tv_sec) && (tv_nsec == other.tv_nsec));
}

bool CTime::operator!=(const CTime& other) const
{
   return ((tv_sec != other.tv_sec) || (tv_nsec != other.tv_nsec));   
}

void CTime::enforceNsecRange()
{
   while(tv_nsec<0)
   {
      tv_nsec += nsecInSec;
      tv_sec--;
   }
   while(tv_nsec >= nsecInSec)
   {
      tv_nsec -= nsecInSec;
      tv_sec++;
   }  
}

std::ostream& operator<<(std::ostream& os, const CTime &time)
{
   os << time.tv_sec << "s " << time.tv_nsec << "ns";
   return os;
}