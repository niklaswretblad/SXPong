#pragma once

#include "Constants.hpp"
#include "Structs.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <iostream>
#include <ws2ipdef.h>

#define MULTICASTADDR "234.0.0.255"

enum struct ConnectionState { NOTCONNECTED, FOUNDIP, CONNECTED };

class Networker
{
public:
	Networker();
	~Networker();

	void sendMsg(Packet packet);
	bool connect(bool asLeftPlayer);
	Packet recvPacket();
	void reset();

private:
	void createSocket();
	void initWinSock();
	void generateUniqueID();
	void initialize();

	std::wstring getWSAErrorString(int errCode);

	void broadcastMulticastMessage();
	void leaveMulticastGroup();

	SOCKET sock;
	int uniqueID, backupID;
	std::string targetIP;
	ConnectionState connectionState;
};

