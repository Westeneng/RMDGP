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
 * File:   mcr.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 18, 2021, 8:25 PM
 */

#include "../socketLib/CUdpMulticastReceiver.h"
//#include "../socketLib/CFdWaiter.h"
//#include "../socketLib/CTime.h"
//#include "../socketLib/CClock.h"
#include <iostream>
#include <string>
#include <sstream>

int main(int argc, char** argv)
{
   if(argc < 4)
   {
      std::cout << "usage: mcr mulicast_address multicast_port source_address\n";
      exit(1);
   }
   CUdpMulticastReceiver receiver;
   const std::string multicastAddress(argv[1]);
   const std::string sourceAddress(argv[3]);

   std::istringstream multicastPortSS(argv[2]);
   int mcPort;
   multicastPortSS >> mcPort;

   try
   {
      std::cout << "open receiver:" << multicastAddress << ":" << mcPort << " source:" << sourceAddress << std::endl;
      receiver.open(multicastAddress, mcPort, sourceAddress);
//      receiver.setNonBlocking();

//      std::cout << "Start waiting\n";
//      CFdWaiter waiter;
//      CTime currentTime;
//      CTime minute(60);
//      waiter.addReadFileDescriptor(&receiver);
//      waiter.waitUntil( CClock::getMonotonicTime(currentTime) + minute );

      std::cout << "Start receiving\n";
      char buffer[256] = {0};
      int result = receiver.receive(buffer, sizeof(buffer));
      std::cout << "result=" << result << " received:'" << buffer << "'" << std::endl;
   }
   catch(std::runtime_error &re)
   {
      std::cout << re.what() << std::endl;
   }
}
