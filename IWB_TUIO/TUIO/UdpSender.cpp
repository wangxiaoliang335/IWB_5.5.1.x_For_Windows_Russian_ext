/*
 TUIO C++ Library
 Copyright (c) 2005-2014 Martin Kaltenbrunner <martin@tuio.org>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3.0 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library.
*/

#include "UdpSender.h"
#include <Windows.h>
using namespace TUIO;

UdpSender::UdpSender() {
	try {
		local = true;
		long unsigned int ip = GetHostByName("localhost");
		socket = new UdpTransmitSocket(IpEndpointName(ip, 3333));
		buffer_size = MAX_UDP_SIZE;
		std::cout << "TUIO/UDP messages to " << "127.0.0.1@3333" << std::endl;

        char szBuf[1024] = { 0 };
        sprintf(szBuf, "111 77 UdpSender 127.0.0.1@3333 ip:%u\n", ip);
        OutputDebugStringA(szBuf);

	} catch (std::exception &) { 
		std::cout << "could not create UDP socket" << std::endl;
		socket = NULL;
	}
}

UdpSender::UdpSender(const char *host, int port) {
	try {
		if ((strcmp(host,"127.0.0.1")==0) || (strcmp(host,"localhost")==0)) {
			local = true;
			buffer_size = MAX_UDP_SIZE;
		} else {
			local = false;
			buffer_size = IP_MTU_SIZE;
		}
		long unsigned int ip = GetHostByName(host);
		socket = new UdpTransmitSocket(IpEndpointName(ip, port));
		std::cout << "TUIO/UDP messages to " << host << "@" << port << std::endl;

        char szBuf[1024] = { 0 };
        sprintf(szBuf, "111 88 UdpSender host:%s, port:%d\n", host, port);
        OutputDebugStringA(szBuf);

	} catch (std::exception &) { 
		std::cout << "could not create UDP socket" << std::endl;
		socket = NULL;
	}
}

UdpSender::UdpSender(const char *host, int port, int size) {
	try {
		if ((strcmp(host,"127.0.0.1")==0) || (strcmp(host,"localhost")==0)) {
			local = true;
		} else local = false;
		long unsigned int ip = GetHostByName(host);
		socket = new UdpTransmitSocket(IpEndpointName(ip, port));
		if (buffer_size>MAX_UDP_SIZE) buffer_size = MAX_UDP_SIZE;
		else if (buffer_size<MIN_UDP_SIZE) buffer_size = MIN_UDP_SIZE;
		std::cout << "TUIO/UDP messages to " << host << "@" << port << std::endl;

        char szBuf[1024] = { 0 };
        sprintf(szBuf, "111 99 UdpSender host:%s, port:%d\n", host, port);
        OutputDebugStringA(szBuf);

	} catch (std::exception &) { 
		std::cout << "could not create UDP socket" << std::endl;
		socket = NULL;
	}
}

UdpSender::~UdpSender() {
	delete socket;		
}

bool UdpSender::isConnected() { 
	if (socket==NULL) return false; 
	return true;
}

bool UdpSender::sendOscPacket (osc::OutboundPacketStream *bundle) {

    char szBuf[1024] = { 0 };
    sprintf(szBuf, "111 55 UdpSender Entry sendOscPacket\n");
    OutputDebugStringA(szBuf);

	if (socket==NULL) return false; 
	if ( bundle->Size() > buffer_size ) return false;
	if ( bundle->Size() == 0 ) return false;

    sprintf(szBuf, "111 66 UdpSender Entry sendOscPacket\n");
    OutputDebugStringA(szBuf);

	socket->Send( bundle->Data(), bundle->Size() );
	return true;
}
