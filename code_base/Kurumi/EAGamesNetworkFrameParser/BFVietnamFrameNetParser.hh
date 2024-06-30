/*
	(C) Keowu - 2024
*/
#pragma once
#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

typedef struct BFVietnamgameServer {

	/*
		Server IP
	*/
	uint32_t dwServerIP;

	/*
		Server Port
	*/
	uint16_t wServerPort;

};

class BFVietnamFrameNetParser {

private:

	/*
		Payload Header signature
	*/
	unsigned char m_payloadSignature[0x25]{ 0 };

	/*
		Requester IP Address
	*/
	uint32_t m_dwRequesterIP{ 0 };

	/*
		Default Gameservers query port
	*/
	uint16_t m_dDefaultPort{ 0 };

	/*
		Custom configuration for gameservers and query information
	*/
	unsigned char m_payloadGameConfiguration[0xC0]{ 0 };

	/*
		List with all servers and new servers into a payload
	*/
	std::vector<BFVietnamgameServer> m_BfVietnamServers;

	/*
		New flag delimiter flag
	*/
	unsigned char m_ucServerDelimiterFlag{ 0x15 };

	/*
		Payload signature flag
	*/
	const unsigned char m_frameEndSignature[5]{ 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

	/*
		Max payload size per server browser query
	*/
	const int m_MAX_PAYLOAD_SIZE{ 4096 };

	/*
		Raw frame payload
	*/
	unsigned char* m_ucRawPayload{ 0 };

	/*
		Raw new frame payload
	*/
	unsigned char* m_ucNewRawPayload{ 0 };

public:
	BFVietnamFrameNetParser(std::string& strFilePath);

	auto getRequesterIp() -> std::string;

	auto setRequesterIp(const char* chIpv4) -> void;

	auto addServer(const char* chIpv4, const char* chPort) -> void;

	auto deleteServer(int index) -> void;

	auto getNewPayload() -> unsigned char*;

	auto getRawPayload() -> unsigned char*;

	auto writeNewPayload(std::string& path) -> void;

	operator std::string() const;

	~BFVietnamFrameNetParser();

};

