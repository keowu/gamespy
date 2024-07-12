/*
	File: HaloCEFrameNetParser.hh
	Author: João Vitor(@Keowu)
	Created: 24/03/2024
	Last Update: 08/07/2024

	Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>

typedef struct HaloCEGameservers {

	/*
		Server IPV4
	*/
	uint32_t dwServerIP;

	/*
		Server Port
	*/
	uint16_t dwServerPort { 0 };

	/*
		Server signature/randon/bytes
	*/
	unsigned char *serverSign;

	/*
		Server signature size, always 10
	*/
	size_t szServerSign;

};

typedef struct HaloCEGameFlags {

	/*
		Flag bytes
	*/
	unsigned char* ucRawFlagBytes;

	/*
		Flag offsetsize - Size of a flag
	*/
	uintptr_t uipServersFrameOffsetSize;

	/*
		All gameservers presents on a flag
	*/
	std::vector< HaloCEGameservers > vecGameServers;

};

class HaloCEFrameNetParser {

private:
	/*
		User IPV4 requested gamespy payload
	*/
	uint32_t m_requesterIP { 0 };

	/*
		The default port configured by the game dev
	*/
	uint16_t m_defaultGameServerPort { 0 };

	/*
		All gamespy gameflags
	*/
	std::vector< HaloCEGameFlags > m_gameFlags;

	/*
		A copy of current mapped payload binary representation
		the state can change based on modification made using this code
	*/
	unsigned char* m_rawBuffer;

	/*
		The max size of the frame
		the state can change based on modification made using this code
	*/
	uint64_t szFrames;

	/*
		The DWORD representing the end of a GS payload
		a correct payload always ends with 0xFFFFFFFF!
	*/
	const uint32_t m_endFrameSignature { 0xFFFFFFFF };

	/*
		The signature to represent the first byte of signature/flags
	*/
	const unsigned char m_firstBlockByte { 0x0A };

	/*
		The flag that indicate a new server entry
	*/
	const unsigned char m_flagNewServer { 0x3F };

	/*
		The max size a payload can have.
	*/
	const int m_MAX_PAYLOAD_SIZE { 4096 };

public:

	HaloCEFrameNetParser(
		
		std::string& strFilePath
	
	);

	auto getRequesterIp(
	
	) -> std::string;
	
	auto setRequesterIp(
		
		const char* chIpv4
	
	) -> void;
	
	auto addServer(
		
		const char* chIpv4,
		const char* chPort
	
	) -> bool;
	
	auto deleteServer(
		
		const char* chIpv4,
		const char* chPort
	
	) -> bool;
	
	auto getNewPayload(
	
	) -> unsigned char*;
	
	auto getRawPayload(
	
	) -> unsigned char*;
	
	auto writeNewPayload(
		
		std::string& path
	
	) -> bool;
	
	operator std::string(
	
	) const;
	
	~HaloCEFrameNetParser(
	
	);

};