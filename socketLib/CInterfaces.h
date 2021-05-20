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
 * File:   CInterfaces.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on May 20, 2021, 10:32 AM
 */

#ifndef CINTERFACES_H
#define CINTERFACES_H

#include <set>
#include <memory>
#include <netinet/in.h>

class CSocketProxy;
struct in_addr;       // include <netinet/in.h>
struct sockaddr_in;   // include <netinet/in.h>

class CInterfaces {
public:
    CInterfaces();
    CInterfaces(std::shared_ptr<CSocketProxy> Proxy);
    CInterfaces(const CInterfaces& orig) = delete;
    virtual ~CInterfaces();

    /// \brief retrieves the interface address from a given address. Looks for an interface
    ///        address with a matching subnet. If no matching address found INADDR_ANY will be
    ///        returned
    /// \throws std::runtime_error when OS reports an error.
    in_addr retrieveInterfaceAdressFromAddress(const in_addr address);

    /// \brief get all IPv4 interface addresses.
    /// \throws std::runtime_error when OS reports an error.
    std::set<in_addr_t> getIpV4InterfaceAddresses();

    /// \brief get all IPv4 interface addresses that match with given address and mask
    /// \param address to match. For instance "192.168.0.0" if you want to get a private interface
    ///        address.
    /// \param mask that determines the part of the address that has to be identical. For instance
    ///        providing "0.0.0.0" will give you all available IPv4 interface addresses.
    /// \throws std::runtime_error when OS reports an error.
    std::set<in_addr_t> getIpV4InterfaceAddresses(const in_addr_t address, const in_addr_t mask);
    std::set<in_addr_t> getIpV4InterfaceAddresses(const in_addr address, const in_addr mask)
        { return getIpV4InterfaceAddresses(address.s_addr, mask.s_addr); }

    /// \brief returns the address of an interface with a private network address.
    /// \throws std::runtime_error when OS reports an error.
    in_addr_t getAPrivateNetworkInterface();

private:
    std::shared_ptr<CSocketProxy> proxy;
};

#endif /* CINTERFACES_H */

