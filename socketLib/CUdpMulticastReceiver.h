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
 * File:   CUdpMulticastReceiver.h
 * Author: G.J. Westeneng (Gerald Westeneng)
 *
 * Created on February 13, 2021, 9:25 PM
 */

#ifndef CUDPMULTICASTRECEIVER_H
#define CUDPMULTICASTRECEIVER_H

#include <memory>
#include <string>
#include <stddef.h>
#include "CUdpSocket.h"

class CInAddr;
struct in_addr;

/// \brief Provides the ability to receive messages from a TCP/UDP multi cast group
class CUdpMulticastReceiver : public CUdpSocket {
public:
    CUdpMulticastReceiver();
    CUdpMulticastReceiver(const CUdpMulticastReceiver& orig);
    virtual ~CUdpMulticastReceiver();
    
    /// \brief Open for receiving. The internal socket will be created and initialized.
    /// \param multicastAddress
    ///        The multi cast address to listen
    /// \param multicastPort
    ///        The multi cast port to listen. This is the same as the local UDP port.
    /// \param sourceAddress
    ///        The IP address of the sender of the datagrams.
    ///        Datagrams from others will not been received.
    ///        But providing the any address ("0.0.0.0" in IPv4) will let you receive messages from
    ///        all senders.
    /// \param sourcePort
    ///        The port number of the source. At a receive messages from other ports are filtered
    ///        out. If port is 0 messages from all ports are received. When set to -1 the port
    ///        number of the first message will be used to filter. Default value is 0.
    /// \throws std::runtime_error when not able to open and initialize the socket
    void open(const std::string multicastAddress, int multicastPort,
                const std::string sourceAddress, int sourcePort=0);
    
    /// \brief Receive a message from the multi cast address.
    ///        If a message from an other address is received in the buffer, a new receive from
    ///        the socket will be done. When there is no next message in the buffer then in non
    ///        blocking mode 0 will be returned while the buffer will contain the message received
    ///        from the other address.
    /// \return The number of bytes received. 
    ///         In non blocking mode 0 indicates there is no message available.
    /// \throws std::runtime_error when OS reports an error.
    size_t receive(void *buffer, size_t bufferSize);
    
    /// \brief returns the source port number
    int getSourcePortNumber() const;
    /// \brief returns the source ip address
    in_addr getSourceIpAddress() const;
    /// \brief returns the multicast ip address
    std::string getMulticastAddress() const;
    /// \brief returns the multicast port
    int getMulticastPort() const;
    
private:
    std::unique_ptr<in_addr> sourceIpAddress;
    std::string multicastAddress;
    int multicastPort;
    int sourcePortNumber;
};

#endif /* CUDPMULTICASTRECEIVER_H */

