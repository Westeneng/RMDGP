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
 * File:   mcs.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 18, 2021, 8:25 PM
 */

 #include "../socketLib/CUdpMulticastSender.h"
 #include <iostream>
 #include <string>
 #include <sstream>

int main(int argc, char** argv)
{
   if(argc < 5)
   {
      std::cout << "usage: mcs mulicast_address multicast_port if_address message\n";
      exit(1);
   }
   CUdpMulticastSender sender;
   const std::string multicastAddress(argv[1]);
   std::istringstream multicastPortSS(argv[2]);
   const std::string interfaceAddress(argv[3]);
   const std::string message(argv[4]);
   int mcPort;

   multicastPortSS >> mcPort;

   try
   {
      std::cout << "open sender:" << multicastAddress << ":" << mcPort << " if:" << interfaceAddress << std::endl;
      sender.open(multicastAddress, mcPort, interfaceAddress);
      std::cout << "send:'" << message << "'" << std::endl;
      sender.send(message.c_str(), message.size());
   }
   catch(std::runtime_error &re)
   {
      std::cout << re.what() << std::endl;
   }
}
