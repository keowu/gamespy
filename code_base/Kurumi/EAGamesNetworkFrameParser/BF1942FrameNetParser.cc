/*
	File: BF1942FrameNetParser.cc
	Author: João Vitor(@Keowu)
	Created: 16/03/2024
	Last Update: 09/07/2024

	Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include "BF1942FrameNetParser.hh"


BF1942FrameNetParser::BF1942FrameNetParser( 
	
	std::string& strFilePath

) {

	std::ifstream in( strFilePath, std::ios::binary | std::ios::ate );

	if ( !in.is_open( ) ) throw new std::exception( "Open file failed!" );

	auto szFile = in.tellg( );

	this->m_ucRawFrame = new unsigned char [ szFile ] { 0 };

	in.seekg( 0, std::ios::beg );

	in.read( reinterpret_cast< char* >( this->m_ucRawFrame ), szFile );

	in.close( );

	std::memcpy( this->m_ucFrameSignature, this->m_ucRawFrame, sizeof( this->m_ucFrameSignature ) );

	auto ptr = this->m_ucRawFrame;

	ptr += sizeof( this->m_ucFrameSignature );

	while ( std::memcmp( ptr, m_endSignature, sizeof( m_endSignature ) ) != 0 ) {

		BF1942gameServers srv{ 0 };

		std::memcpy( &srv.dwServerIP, ptr, sizeof( uint32_t ) );

		ptr += sizeof( uint32_t );

		std::memcpy( &srv.dwServerPort, ptr, sizeof( uint16_t ) );

		ptr += sizeof( uint16_t );

		this->m_servers.push_back( srv );

	}

}

auto BF1942FrameNetParser::removeServer( 
	
	int index

) -> void {

	if ( index > this->m_servers.size( ) ) return;

	this->m_servers.erase( this->m_servers.begin( ) + index );

}

auto BF1942FrameNetParser::checkServerStatus( 
	
	BF1942gameServers* bfg

) -> bool {

	WSADATA wsaData;

	if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) return false;

	SOCKET sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( sockfd == INVALID_SOCKET ) {

		WSACleanup( );

		return false;
	}

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;

	serverAddr.sin_port = htons( _byteswap_ushort( bfg->dwServerPort ) );
	serverAddr.sin_addr.s_addr =  bfg->dwServerIP;

	if ( sendto(sockfd, this->m_stausQuery, strnlen_s( this->m_stausQuery, 20 ), 0, reinterpret_cast< struct sockaddr* >( &serverAddr ), sizeof( serverAddr ) ) == SOCKET_ERROR ) {
	
		closesocket( sockfd );

		WSACleanup( );

		return false;
	}


	struct timeval tv;
	tv.tv_sec = 20;
	tv.tv_usec = 0;

	if ( setsockopt( sockfd, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast< const char* >( &tv ), sizeof( tv ) ) < 0 ) {

		closesocket( sockfd );

		WSACleanup( );

		return false;
	}

	auto chBufferStatus = new char[ MAX_STATUS_SIZE ] { 0 };
	int msForcingAPointer = sizeof( serverAddr );

	auto recvLen = recvfrom( sockfd, chBufferStatus, MAX_STATUS_SIZE, 0, reinterpret_cast< struct sockaddr* >( &serverAddr ), &msForcingAPointer );

	if ( recvLen == SOCKET_ERROR ) {

		closesocket( sockfd );

		WSACleanup( );

		return false;
	}

	*( chBufferStatus + recvLen ) = '\0';

	bfg->status_response = std::string( chBufferStatus );

	closesocket( sockfd );
	WSACleanup( );

	return true;
}

auto BF1942FrameNetParser::addNewServer( 
	
	const char* chSeverIp,
	const char* chSeverPort,
	bool check

) -> bool {

	BF1942gameServers srv { };

	int b1 { 0 }, b2 { 0 }, b3 { 0 }, b4 { 0 }, port { 0 };

	sscanf_s( chSeverIp, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 );
	sscanf_s( chSeverPort, "%d", &port );

	srv.dwServerIP = ( static_cast< uint32_t >( b4 ) << 24) |
		( static_cast< uint32_t >( b3 ) << 16) |
		( static_cast< uint32_t >( b2 ) << 8) |
		static_cast< uint32_t >( b1 );

	srv.dwServerPort = _byteswap_ushort( static_cast< uint16_t >( port ) );

	if ( check && this->checkServerStatus( &srv ) ) return false;

	std::printf( "[OK] Success adding -> %s:%s\n", chSeverIp, chSeverPort );

	this->m_servers.push_back( srv );

	return true;

}

auto BF1942FrameNetParser::getNewPayload(

) -> unsigned char* {

	if ( this->m_ucNewFrame ) delete this->m_ucNewFrame;

	this->m_ucNewFrame = new unsigned char[ MAX_PAYLOAD_SIZE ] { 0 };

	std::memcpy( this->m_ucNewFrame, this->m_ucFrameSignature, sizeof( this->m_ucFrameSignature ) );

	auto ptr = this->m_ucNewFrame;
	ptr += sizeof( this->m_ucFrameSignature );

	for ( auto server : this->m_servers ) {

		std::memcpy( ptr, &server.dwServerIP, sizeof( uint32_t ) );
		ptr += sizeof( uint32_t );
		std::memcpy( ptr, &server.dwServerPort, sizeof( uint16_t ) );
		ptr += sizeof( uint16_t );

	}

	std::memcpy( ptr, m_endSignature, strnlen_s( m_endSignature, 10 ) );

	return this->m_ucNewFrame;
}

auto BF1942FrameNetParser::writeNewPayload( 
	
	std::string& path

) -> bool {

	std::ofstream out( path, std::ios::binary );

	if ( !out ) return false;

	out.write( reinterpret_cast< char* >( this->getNewPayload( ) ), MAX_PAYLOAD_SIZE );

	out.close( );

	return true;
}

auto BF1942FrameNetParser::getRawPayload( 

) -> unsigned char* {

	return this->m_ucRawFrame;
}

BF1942FrameNetParser::operator std::string( 

) const {

	std::ostringstream out;

	for ( const auto& bfServer : this->m_servers )

		out << ( bfServer.dwServerIP & 0xff ) << "."
			<< ( ( bfServer.dwServerIP >> 8 ) & 0xff ) << "."
			<< ( ( bfServer.dwServerIP >> 16 ) & 0xff ) << "."
			<< ( ( bfServer.dwServerIP >> 24 ) & 0xff ) << ":"
			<< _byteswap_ushort( bfServer.dwServerPort ) << "\n";

	return out.str( );
}

BF1942FrameNetParser::~BF1942FrameNetParser(

) {

	this->m_servers.clear( );

	if ( this->m_ucNewFrame ) delete this->m_ucNewFrame;

	if ( this->m_ucRawFrame ) delete this->m_ucRawFrame;

}