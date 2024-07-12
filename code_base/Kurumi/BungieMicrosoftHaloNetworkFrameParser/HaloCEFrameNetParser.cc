/*
	File: HaloCEFrameNetParser.cc
	Author: João Vitor(@Keowu)
	Created: 24/03/2024
	Last Update: 09/07/2024

	Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include "HaloCEFrameNetParser.hh"

HaloCEFrameNetParser::HaloCEFrameNetParser(
	
	std::string& strFilePath

) {

	std::ifstream in( strFilePath, std::ios::binary | std::ios::ate );

	if ( !in.is_open( ) ) throw new std::exception( "Open file failed!" );

	auto szFile = in.tellg( );

	this->szFrames = szFile;

	this->m_rawBuffer = new unsigned char[ szFile ] { 0 };

	in.seekg( 0, std::ios::beg );

	in.read( reinterpret_cast< char* >( this->m_rawBuffer ), szFile );

	in.close( );

	auto tempPtr = this->m_rawBuffer;

	std::memcpy( &this->m_requesterIP, tempPtr, sizeof( uint32_t ) );

	tempPtr += sizeof( uint32_t );

	std::memcpy( &this->m_defaultGameServerPort, tempPtr, sizeof( uint16_t ) );

	tempPtr += sizeof( uint16_t );

	if ( *tempPtr != this->m_firstBlockByte ) throw new std::exception( "The standard byte for the first flags did not match!" );

	//Locating the frames and parse each server too
	auto i = 0;
	auto gameFlagsPtr = tempPtr;

	while ( i <= this->szFrames ) {

		HaloCEGameFlags newFlag;
		
		uintptr_t offset = 0;

		while ( true ) {

			if ( gameFlagsPtr[ offset ] == this->m_flagNewServer ) break;

			offset += 1;
		}

		newFlag.ucRawFlagBytes = new unsigned char[ offset ] { 0 };
		
		std::memcpy( newFlag.ucRawFlagBytes, gameFlagsPtr, offset );

		newFlag.uipServersFrameOffsetSize = offset;

		//skipping the servers block to go to new flags block
		while ( true ) {

			offset += 1; // skip 0x3F flag

			HaloCEGameservers newServer;

			std::memcpy( &newServer.dwServerIP, &gameFlagsPtr[ offset ], sizeof( uint32_t ) );

			offset += sizeof( uint32_t );

			std::memcpy( &newServer.dwServerPort, &gameFlagsPtr[ offset ], sizeof( uint16_t ) );

			offset += sizeof( uint16_t );

			auto sz = 10;

			newServer.serverSign = new unsigned char[ sz ] { 0 };

			newServer.szServerSign = sz;

			std::memcpy( newServer.serverSign, &gameFlagsPtr[ offset ], sz );

			offset -= 1; // return to last byte of port

			offset += 11; //calc next flag

			newFlag.vecGameServers.push_back( newServer );

			if ( gameFlagsPtr[ offset ] != this->m_flagNewServer ) break;

		}
		
		this->m_gameFlags.push_back( newFlag );

		gameFlagsPtr += offset;
		i += offset;
	}

	this->m_gameFlags.pop_back( );
}

auto HaloCEFrameNetParser::getNewPayload(

) -> unsigned char* {

	auto chBuffer = new unsigned char[ m_MAX_PAYLOAD_SIZE ] { 0 };

	auto tempPtr = chBuffer;

	std::memcpy( tempPtr, &this->m_requesterIP, sizeof( uint32_t ) );

	tempPtr += sizeof( uint32_t );

	std::memcpy( tempPtr, &this->m_defaultGameServerPort, sizeof( uint16_t ) );

	tempPtr += sizeof( uint16_t );

	for ( auto& flag : this->m_gameFlags ) {

		std::memcpy( tempPtr, flag.ucRawFlagBytes, flag.uipServersFrameOffsetSize );

		tempPtr += flag.uipServersFrameOffsetSize;

		for ( auto& servers : flag.vecGameServers ) {

			std::memcpy( tempPtr, &this->m_flagNewServer, sizeof( unsigned char ) );

			tempPtr += sizeof( unsigned char );

			std::memcpy( tempPtr, &servers.dwServerIP, sizeof( uint32_t ) );

			tempPtr += sizeof( uint32_t );

			std::memcpy( tempPtr, &servers.dwServerPort, sizeof( uint16_t ) );

			tempPtr += sizeof( uint16_t );

			std::memcpy( tempPtr, servers.serverSign, servers.szServerSign );

			tempPtr += servers.szServerSign;

		}

	}

	unsigned char chZeroed { 0x00 };

	std::memcpy( tempPtr, &chZeroed, sizeof( unsigned char ) );

	tempPtr += 1;

	std::memcpy( tempPtr, &this->m_endFrameSignature, sizeof( uint32_t ) );

	delete this->m_rawBuffer;

	this->m_rawBuffer = chBuffer;

	return chBuffer;
}

HaloCEFrameNetParser::operator std::string(

) const {

	std::string out( "" );

	for ( const auto& gameFlags : this->m_gameFlags ) {

		for ( const auto& gameServer : gameFlags.vecGameServers ) {

			std::ostringstream oss;
			
			oss << ( gameServer.dwServerIP & 0xff ) << '.'
				<< ( ( gameServer.dwServerIP >> 8 ) & 0xff ) << '.'
				<< ( ( gameServer.dwServerIP >> 16 ) & 0xff ) << '.'
				<< ( ( gameServer.dwServerIP >> 24 ) & 0xff ) << ':'
				<< _byteswap_ushort( gameServer.dwServerPort ) << '\n';

			out.append( oss.str( ) );
		}
	}

	return out;
}

auto HaloCEFrameNetParser::getRequesterIp(

) -> std::string {

	std::ostringstream oss;

	oss << ( this->m_requesterIP & 0xff ) << '.'
		<< ( ( this->m_requesterIP >> 8 ) & 0xff ) << '.'
		<< ( ( this->m_requesterIP >> 16 ) & 0xff ) << '.'
		<< ( ( this->m_requesterIP >> 24 ) & 0xff ) << '\n';

	return oss.str( );
}

auto HaloCEFrameNetParser::setRequesterIp(
	
	const char* chIpv4

) -> void {

	int b1 { 0 }, b2 { 0 }, b3 { 0 }, b4 { 0 };

	sscanf_s( chIpv4, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 );

	this->m_requesterIP = ( static_cast< uint32_t >( b4 ) << 24 ) |
		( static_cast< uint32_t >( b3 ) << 16 ) |
		( static_cast< uint32_t >( b2 ) << 8 ) |
		static_cast< uint32_t >( b1 );

	std::cout << "[OK] The requester IP Address has been changed\n";
}

auto HaloCEFrameNetParser::addServer(
	
	const char* chIpv4,
	const char* chPort

) -> bool {

	int b1 { 0 }, b2 { 0 }, b3 { 0 }, b4 { 0 }, port { 0 };

	sscanf_s( chIpv4, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 );

	sscanf_s( chPort, "%d", &port );

	auto ip = ( static_cast< uint32_t >( b4 ) << 24) |
		( static_cast< uint32_t >( b3 ) << 16) |
		( static_cast< uint32_t >( b2 ) << 8) |
		static_cast< uint32_t >( b1 );

	auto swappedPort = _byteswap_ushort( static_cast< uint16_t >( port ) );

	for ( auto& gameFlags : this->m_gameFlags ) {

		for ( auto& server : gameFlags.vecGameServers ) {

			if ( server.dwServerIP == 0 ) {

				server.dwServerIP = ip;
				server.dwServerPort = swappedPort;

				return true;
			}
		}
	}

	return false;
}

auto HaloCEFrameNetParser::deleteServer(
	
	const char* chIpv4,
	const char* chPort

) -> bool {

	int b1 { 0 }, b2 { 0 }, b3 { 0 }, b4 { 0 }, port { 0 };

	sscanf_s( chIpv4, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 );

	sscanf_s( chPort, "%d", &port );

	auto ip = ( static_cast< uint32_t >( b4 ) << 24 ) |
		( static_cast< uint32_t >( b3 ) << 16 ) |
		( static_cast< uint32_t >( b2 ) << 8 ) |
		static_cast< uint32_t >( b1 );

	auto swappedPort = _byteswap_ushort( static_cast< uint16_t >( port ) );

	for ( auto& gameFlags : this->m_gameFlags ) {

		for ( auto& server : gameFlags.vecGameServers ) {

			if ( server.dwServerIP == ip && server.dwServerPort == swappedPort ) {

				server.dwServerIP = 0;
				server.dwServerPort = 0;

				return true;
			}
		}
	}

	return false;
}

auto HaloCEFrameNetParser::getRawPayload(

) -> unsigned char* {

	return this->m_rawBuffer;
}

auto HaloCEFrameNetParser::writeNewPayload(
	
	std::string& path

) -> bool {

	std::ofstream out( path, std::ios::binary );

	if ( !out ) return false;

	out.write( reinterpret_cast< char* >( this->getNewPayload( ) ), this->m_MAX_PAYLOAD_SIZE );

	out.close( );

	return false;
}

HaloCEFrameNetParser::~HaloCEFrameNetParser(

) {

	this->m_gameFlags.clear( );

	delete this->m_rawBuffer;

}