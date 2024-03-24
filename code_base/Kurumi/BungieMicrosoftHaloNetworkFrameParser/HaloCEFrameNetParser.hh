/*
	(C) Keowu - 2024
*/
#include <iostream>
#include <vector>
#include <fstream>

typedef struct HaloCEGameservers {

	uint32_t dwServerIP;

	uint16_t dwServerPort{ 0 };

	unsigned char serverSign[10];

};

typedef struct HaloCEGameFlags {

	unsigned char* ucRawFlagBytes;
	uintptr_t uipServersFrameOffset;
	std::vector<HaloCEGameservers> vecGameServers;

};

class HaloCEFrameNetParser {

private:
	uint32_t m_requesterIP{ 0 };
	uint16_t m_defaultGameServerPort{ 0 };
	std::vector<HaloCEGameFlags> m_gameFlags;
	unsigned char* m_rawBuffer;
	uint64_t szFrames;
	const uint32_t m_endFrameSignature{ 0xFFFFFFFF };
	const unsigned char m_firstBlockByte{ 0x0A };
	const unsigned char m_flagNewServer{ 0x3F };


public:
	HaloCEFrameNetParser(std::string& strFilePath);
	operator std::string() const;
	~HaloCEFrameNetParser();

};