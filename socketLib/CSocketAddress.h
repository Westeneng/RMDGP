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
 * File:   CSocketAddress.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 7, 2021, 1:57 PM
 */

#ifndef CSOCKETADDRESS_H
#define CSOCKETADDRESS_H

#include <netinet/in.h>
#include <arpa/inet.h>

/// \brief encapsulates sockaddr_in
class CSocketAddress : public sockaddr_in {
public:
    CSocketAddress(const char *ipAddress, int port)
    {
        // initialize structure with zero
        *((sockaddr_in *)this) = {0};
        sin_family = AF_INET;
        sin_addr.s_addr = inet_addr(ipAddress);
        sin_port = htons(port);
    }
};

#endif /* CSOCKETADDRESS_H */

