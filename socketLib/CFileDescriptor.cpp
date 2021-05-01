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
 * File:   CFileDescriptor.cpp
 * Author: G.J. Westeneng (Gerald Westeneng)
 * 
 * Created on April 29, 2021, 7:42 PM
 */

#include "CFileDescriptor.h"
#include "CSocketProxy.h"        // includes sys/types.h and sys/socket.h

CFileDescriptor::CFileDescriptor() : sfd(-1), proxy(new CSocketProxy())
{
}

CFileDescriptor::CFileDescriptor(std::shared_ptr<CSocketProxy> sockProxy) : sfd(-1), proxy(sockProxy)
{
}

CFileDescriptor::~CFileDescriptor()
{
   if(sfd >= 0) proxy->close(sfd);
   sfd = -1;
}

void CFileDescriptor::setSocketProxy(std::shared_ptr<CSocketProxy> sockProxy)
{
   proxy = sockProxy;
}
