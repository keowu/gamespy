/*
	(C) Keowu - 2024
*/
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include "TeaDelKewAlgo.hh"

#pragma comment(lib, "ws2_32.lib")

typedef struct BF1942gameServers {

	/*
		IPV4
	*/
	uint32_t dwServerIP { 0 };

	/*
		PORT
	*/
	uint16_t dwServerPort { 0 };

	/*
		Data to Display on SERVER BROWSER
	*/
	std::string status_response;

};

class BF1942FrameNetParser {

private:
	
	unsigned char m_ucFrameSignature[13]{ 0 };
	std::vector<BF1942gameServers> m_servers;
	const char* m_endSignature{ "\\final\\" };
	const char* m_stausQuery{ "\\status\\" };
	const int MAX_STATUS_SIZE{ 0x5DB };
	unsigned char* m_ucRawFrame{ NULL };
	unsigned char* m_ucNewFrame{ NULL };
	const int MAX_PAYLOAD_SIZE{ 0x7FF }; // MAX That Battlefield server list can display

public:

	BF1942FrameNetParser( std::string& strFilePath );

	auto addNewServer( const char* chSeverIp, const char* chSeverPort, bool check=true ) -> bool;

	auto removeServer( int index ) -> void;

	auto getNewPayload( ) -> unsigned char*;

	auto getRawPayload( ) -> unsigned char*;

	auto writeNewPayload( std::string& path ) -> bool;

	auto checkServerStatus( BF1942gameServers* bfg ) -> bool;

	operator std::string( ) const;
	
	~BF1942FrameNetParser( );

};

