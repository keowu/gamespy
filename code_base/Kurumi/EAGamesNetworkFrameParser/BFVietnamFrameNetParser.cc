/*
	File: BFVietnamFrameNetParser.cc
	Author: João Vitor(@Keowu)
	Created: 26/04/2024
	Last Update: 09/07/2024

	Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include "BFVietnamFrameNetParser.hh"

BFVietnamFrameNetParser::BFVietnamFrameNetParser(
	
	std::string& strFilePath

) {

	std::ifstream in( strFilePath, std::ios::binary | std::ios::ate );

	if ( !in.is_open( ) ) throw new std::exception( "Open file failed!" );

	auto szFile = in.tellg( );

	this->m_ucRawPayload = new unsigned char [ szFile ] { 0 };

	in.seekg( 0, std::ios::beg );

	in.read( reinterpret_cast< char* >( this->m_ucRawPayload ), szFile );

	in.close( );

	auto pSeek = this->m_ucRawPayload;

	std::memcpy( this->m_payloadSignature, pSeek, sizeof( this->m_payloadSignature ) );

	pSeek += sizeof( this->m_payloadSignature );

	std::memcpy( &this->m_dwRequesterIP, pSeek, sizeof( this->m_dwRequesterIP ) );

	pSeek += sizeof( this->m_dwRequesterIP );

	std::memcpy( &this->m_dDefaultPort, pSeek, sizeof( this->m_dDefaultPort ) );

	pSeek += sizeof( this->m_dDefaultPort );

	std::memcpy( this->m_payloadGameConfiguration, pSeek, sizeof( this->m_payloadGameConfiguration ) );

	pSeek += sizeof( this->m_payloadGameConfiguration ) + 1; // +1 for ignore (magic flag 0x15) the first server delimiter

	//Parsing the servers from payload
	while ( std::memcmp( pSeek, m_frameEndSignature, 5 ) != 0 ) {

		if ( std::memcmp( pSeek, &this->m_ucServerDelimiterFlag, 1 ) == 0 ) pSeek += 1;

		BFVietnamgameServer bfVs;
		
		std::memcpy( &bfVs.dwServerIP, pSeek, sizeof( bfVs.dwServerIP ) );
		
		pSeek += sizeof( bfVs.dwServerIP );

		std::memcpy( &bfVs.wServerPort, pSeek, sizeof( bfVs.wServerPort ) );

		pSeek += sizeof( bfVs.wServerPort );

		this->m_BfVietnamServers.push_back( bfVs );

	}

}

auto BFVietnamFrameNetParser::getRequesterIp(

) -> std::string {

	char chRequesterIp[ 64 ] { 0 };

	sprintf_s( chRequesterIp, "%d.%d.%d.%d\n",
		this->m_dwRequesterIP & 0xff,
		( this->m_dwRequesterIP >> 8 ) & 0xff,
		( this->m_dwRequesterIP >> 16 ) & 0xff,
		( this->m_dwRequesterIP >> 24 ) & 0xff );

	return std::string( chRequesterIp );
}

auto BFVietnamFrameNetParser::setRequesterIp(
	
	const char* chIpv4

) -> void {

	int b1 { 0 }, b2 { 0 }, b3 { 0 }, b4 { 0 }, port { 0 };

	sscanf_s( chIpv4, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 );

	this->m_dwRequesterIP = ( static_cast< uint32_t >( b4 ) << 24 ) |
		( static_cast< uint32_t >( b3 ) << 16 ) |
		( static_cast< uint32_t >( b2 ) << 8 ) |
		static_cast< uint32_t >( b1 );

}

auto BFVietnamFrameNetParser::addServer(
	
	const char* chIpv4,
	const char* chPort

) -> void {

	int b1 { 0 }, b2 { 0 }, b3 { 0 }, b4 { 0 }, port { 0 };

	sscanf_s( chIpv4, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 );

	sscanf_s( chPort, "%d", &port );

	BFVietnamgameServer bfV { };

	bfV.dwServerIP = ( static_cast< uint32_t >( b4 ) << 24 ) |
		( static_cast< uint32_t >( b3 ) << 16 ) |
		( static_cast< uint32_t >( b2 ) << 8 ) |
		static_cast< uint32_t >( b1 );

	bfV.wServerPort = _byteswap_ushort( static_cast< uint16_t >( port ) );

	this->m_BfVietnamServers.push_back( bfV );
}

auto BFVietnamFrameNetParser::deleteServer(
	
	int index

) -> void {

	if ( index > this->m_BfVietnamServers.size( ) ) return;

	this->m_BfVietnamServers.erase( this->m_BfVietnamServers.begin( ) + index );

}

auto BFVietnamFrameNetParser::getNewPayload(

) -> unsigned char* {

	if ( this->m_ucNewRawPayload ) delete this->m_ucNewRawPayload;

	this->m_ucNewRawPayload = new unsigned char [ m_MAX_PAYLOAD_SIZE ] { 0 };

	auto pSeek = this->m_ucNewRawPayload;

	std::memcpy( pSeek, this->m_payloadSignature, sizeof( m_payloadSignature ) );

	pSeek += sizeof( this->m_payloadSignature );

	std::memcpy( pSeek, &this->m_dwRequesterIP, sizeof( this->m_dwRequesterIP ) );

	pSeek += sizeof( this->m_dwRequesterIP );

	std::memcpy( pSeek, &this->m_dDefaultPort, sizeof( this->m_dDefaultPort ) );

	pSeek += sizeof( this->m_dDefaultPort );

	std::memcpy( pSeek, this->m_payloadGameConfiguration, sizeof( this->m_payloadGameConfiguration ) );

	pSeek += sizeof( this->m_payloadGameConfiguration );

	//First server delemiter flag
	std::memcpy( pSeek, &this->m_ucServerDelimiterFlag, sizeof( this->m_ucServerDelimiterFlag ) );

	pSeek += sizeof( this->m_ucServerDelimiterFlag );

	for ( auto server : this->m_BfVietnamServers ) {

		std::memcpy( pSeek, &server.dwServerIP, sizeof( server.dwServerIP ) );

		pSeek += sizeof( server.dwServerIP );

		std::memcpy( pSeek, &server.wServerPort, sizeof( server.wServerPort ) );

		pSeek += sizeof( server.wServerPort );

		std::memcpy( pSeek, &this->m_ucServerDelimiterFlag, sizeof( this->m_ucServerDelimiterFlag ) );

		pSeek += sizeof( this->m_ucServerDelimiterFlag );

	}

	//We do not care about last delimiter flag
	pSeek -= 1;

	std::memcpy( pSeek, this->m_frameEndSignature, sizeof( this->m_frameEndSignature ) );

	return this->m_ucNewRawPayload;
}

auto BFVietnamFrameNetParser::getRawPayload(

) -> unsigned char* {

	return this->m_ucRawPayload;
}

auto BFVietnamFrameNetParser::writeNewPayload(
	
	std::string& path

) -> void {

	std::ofstream out( path, std::ios::binary );

	if ( !out ) return;

	out.write( reinterpret_cast< char* >( this->getNewPayload( ) ), m_MAX_PAYLOAD_SIZE );

	out.close( );
}

BFVietnamFrameNetParser::operator std::string(

) const {

	std::ostringstream out;

	for ( auto bfServer : this->m_BfVietnamServers )

		out << ( bfServer.dwServerIP & 0xff ) << "."
			<< ( ( bfServer.dwServerIP >> 8 ) & 0xff ) << "."
			<< ( ( bfServer.dwServerIP >> 16 ) & 0xff ) << "."
			<< ( ( bfServer.dwServerIP >> 24 ) & 0xff ) << ":"
			<< _byteswap_ushort( bfServer.wServerPort ) << "\n";

	return out.str( );
}

BFVietnamFrameNetParser::~BFVietnamFrameNetParser(

) {

	this->m_BfVietnamServers.clear( );

	if ( this->m_ucRawPayload ) delete this->m_ucRawPayload;
		 
	if ( this->m_ucNewRawPayload ) delete this->m_ucNewRawPayload;

}