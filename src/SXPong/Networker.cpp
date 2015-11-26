#include "stdafx.h"
#include "Networker.hpp"

Networker::Networker() :
connectionState(ConnectionState::NOTCONNECTED)
{
	initialize();
}

Networker::~Networker()
{
	
	WSACleanup();
}

void Networker::sendMsg(Packet packet)
{
	sockaddr_in sender_addr;
	sender_addr.sin_family = AF_INET;
	sender_addr.sin_port = htons(PORT);
	sender_addr.sin_addr.s_addr = inet_addr(targetIP.c_str());

	packet.uniqueID = uniqueID;

	int iResult = sendto(sock,
		reinterpret_cast<char*>(&packet),
		sizeof(packet),
		0,
		(SOCKADDR *)&sender_addr,
		sizeof(sender_addr));

	if (iResult == SOCKET_ERROR)
	{
		std::wstring errStr = getWSAErrorString(WSAGetLastError());
		std::cout << "Failed to send packet with message: " << 
			std::string(errStr.begin(), errStr.end()) << "\n";
	}
}

Packet Networker::recvPacket()
{
	sockaddr_in sender_addr;
	int sender_addrLen = sizeof(sender_addr);

	Packet recvdPacket = {};

	int iResult = recvfrom(sock,
		reinterpret_cast<char*>(&recvdPacket),
		sizeof(recvdPacket),
		0,
		(SOCKADDR*)&sender_addr,
		&sender_addrLen);

	if (iResult > 0)
	{
		if (recvdPacket.uniqueID != uniqueID)
		{
			if (targetIP.empty())
			{
				if (recvdPacket.message == CONNECTIONBROADCASTMSG || 
					recvdPacket.message == FOUNDYOUMSG)
				{
					targetIP = inet_ntoa(sender_addr.sin_addr); 
					std::cout << "Recieved target ip address: " << targetIP << "\n";
				}
			}
			return recvdPacket;
		}
	}
	return Packet{};
}

bool Networker::connect(bool asLeftPlayer)
{
	if (asLeftPlayer)
	{
		switch (connectionState)
		{
		case ConnectionState::NOTCONNECTED:
		{
			// Broadcast
			broadcastMulticastMessage();

			Packet recvdPacket = recvPacket();

			// Handle response
			if (recvdPacket.message == FOUNDYOUMSG && 
				!targetIP.empty())
			{
				connectionState = ConnectionState::FOUNDIP;
				std::cout << "Connection state: FOUNDIP" << "\n";
			}
			break;
		}
		case ConnectionState::FOUNDIP:
		{
			// Send acc to opponent
			Packet packet = { 0 };
			packet.message = FOUNDYOUMSG;
			sendMsg(packet);

			Packet recvdPacket = recvPacket();
			
			if (recvdPacket.message == WAITINGFORSTARTMSG)
			{
				connectionState = ConnectionState::CONNECTED;
				std::cout << "Connection state: CONNECTED" << "\n";
			}
			break;
		}
		case ConnectionState::CONNECTED:
			return true;
			break;
		}
	}
	else
	{
		switch (connectionState)
		{
		case ConnectionState::NOTCONNECTED:
		{
			// Listen for broadcast
			Packet recvdPacket = recvPacket();

			if (recvdPacket.message == CONNECTIONBROADCASTMSG &&
				!targetIP.empty())
			{
				std::cout << "Connection state: FOUNDIP" << "\n";
				connectionState = ConnectionState::FOUNDIP;
			}
			break;
		}
		case ConnectionState::FOUNDIP:
		{
			// Answer broadcast
			Packet packet = { 0 };
			packet.message = FOUNDYOUMSG;
			sendMsg(packet);

			// Listen for ack
			Packet recvdPacket = recvPacket();
			if (recvdPacket.message == FOUNDYOUMSG)
			{
				connectionState = ConnectionState::CONNECTED;
				std::cout << "Connection state: CONNECTED" << "\n";
			}
			break;
		}
		case ConnectionState::CONNECTED:
			return true;
			break;
		default:
			break;
		}
	}
	return false;
}

void Networker::broadcastMulticastMessage()
{
	Packet packet = { 0 };
	packet.message = CONNECTIONBROADCASTMSG;
	packet.uniqueID = uniqueID;

	sockaddr_in send_addr;
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(PORT);
	send_addr.sin_addr.s_addr = inet_addr(MULTICASTADDR); 
	

	int iResult = sendto(sock,
		reinterpret_cast<char*>(&packet),
		sizeof(Packet),
		0,
		(SOCKADDR *)&send_addr,
		sizeof(send_addr));

	if (iResult == SOCKET_ERROR)
	{
		printf("Failed to send broadcastmsg\n");
		closesocket(sock);
		WSACleanup();
	}
	int i = 1;
}

void Networker::initWinSock()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		std::cout << "Failed WSAStartup" << "\n";
	}
}

void Networker::createSocket()
{

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		printf("Error: socket creation&s\n", getWSAErrorString(WSAGetLastError()));
	}
	
	struct sockaddr_in multicastAddr;
	memset(&multicastAddr, 0, sizeof(sockaddr_in));
	multicastAddr.sin_family = AF_INET;
	multicastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	multicastAddr.sin_port = htons(PORT);

	if (bind(sock, (SOCKADDR*)&multicastAddr, sizeof(multicastAddr)) < 0)
	{
		WSACleanup();
		closesocket(sock);
		printf("Error: socket binding\n");
	}

	struct ip_mreq multicastRequest;
	memset(&multicastRequest, 0, sizeof(ip_mreq));
	multicastRequest.imr_multiaddr.s_addr = inet_addr(MULTICASTADDR);
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&multicastRequest, sizeof(multicastRequest)) < 0)
	{
		WSACleanup();
		closesocket(sock);
		printf("Error: joining multicast group\n");
	}
	
	char ttl = 7;

	if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) < 0)
	{
		printf("Error: setting ttl\n");
	}

	u_long nonblocking = TRUE;

	if (ioctlsocket(sock, FIONBIO, &nonblocking) != 0)
	{
		closesocket(sock);
		WSACleanup();
		printf("Error: setting socket nonblocking\n");
	}
}

void Networker::leaveMulticastGroup()
{
	struct ip_mreq multicastRequest;
	memset(&multicastRequest, 0, sizeof(ip_mreq));
	multicastRequest.imr_multiaddr.s_addr = inet_addr(MULTICASTADDR);
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&multicastRequest, sizeof(multicastRequest)) < 0)
	{
		std::cout << "Failed to drop multicast membership" << std::endl;
	}
}

void Networker::initialize()
{
	initWinSock();
	createSocket();
	generateUniqueID();
}

void Networker::generateUniqueID()
{
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist127(1, 5000);
	uniqueID = dist127(rng);
}

std::wstring Networker::getWSAErrorString(int errCode)
{
	LPWSTR errString = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM, 
		0,      
		errCode, 
		0, 
		(LPWSTR)&errString, 
		0,                 
		0);

	return std::wstring(errString);
}

void Networker::reset()
{
	connectionState = ConnectionState::NOTCONNECTED;
	targetIP.clear();
}

