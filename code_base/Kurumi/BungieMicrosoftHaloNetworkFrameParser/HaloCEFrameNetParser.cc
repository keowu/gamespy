/*
	(C) Keowu - 2024
*/
#include "HaloCEFrameNetParser.hh"

HaloCEFrameNetParser::HaloCEFrameNetParser(std::string& strFilePath) {

	std::ifstream in(strFilePath, std::ios::binary | std::ios::ate);

	if (!in.is_open()) throw new std::exception("Open file failed!");

	auto szFile = in.tellg();

	this->szFrames = szFile;

	this->m_rawBuffer = new unsigned char[szFile] { 0 };

	in.seekg(0, std::ios::beg);

	in.read(reinterpret_cast<char*>(this->m_rawBuffer), szFile);

	in.close();

	auto tempPtr = this->m_rawBuffer;

	std::memcpy(&this->m_requesterIP, tempPtr, sizeof(uint32_t));

	tempPtr += sizeof(uint32_t);

	std::memcpy(&this->m_defaultGameServerPort, tempPtr, sizeof(uint16_t));

	tempPtr += sizeof(uint16_t);

	if (*tempPtr != this->m_firstBlockByte) throw new std::exception("The standard byte for the first flags did not match!");

	//Locating the frames and parse each server too
	auto i = 0;
	auto gameFlagsPtr = tempPtr;
	while (i <= this->szFrames) {

		HaloCEGameFlags newFlag;
		
		uintptr_t offset = 0;

		while (true) {

			if (gameFlagsPtr[offset] == this->m_flagNewServer) break;

			offset += 1;
		}

		newFlag.ucRawFlagBytes = new unsigned char[offset];
		
		std::memcpy(newFlag.ucRawFlagBytes, gameFlagsPtr, offset);

		newFlag.uipServersFrameOffsetSize = offset;

		//skipping the servers block to go to new flags block
		while (true) {

			offset += 1; // skip 0x3F flag

			HaloCEGameservers newServer;

			std::memcpy(&newServer.dwServerIP, &gameFlagsPtr[offset], sizeof(uint32_t));

			offset += sizeof(uint32_t);

			std::memcpy(&newServer.dwServerPort, &gameFlagsPtr[offset], sizeof(uint16_t));

			offset += sizeof(uint16_t);

			std::memcpy(newServer.serverSign, &gameFlagsPtr[offset], 10);

			offset -= 1; // return to last byte of port

			offset += 11; //calc next flag

			newFlag.vecGameServers.push_back(newServer);

			if (gameFlagsPtr[offset] != this->m_flagNewServer) break;

		}
		
		this->m_gameFlags.push_back(newFlag);

		gameFlagsPtr += offset;
		i += offset;
	}

	/*
	* Talvez tenha um bug nessa formatação de um ip randomico com porta randomica por alguma rasão desconhecida
	* mas pode ser apenas coisa da minha cabeça cansada.
	*/
}


HaloCEFrameNetParser::operator std::string() const {

	std::string out;

	for (auto& gameFlags : this->m_gameFlags) {

		for (auto& gameServer : gameFlags.vecGameServers) {

			char chServerString[64]{ 0 };

			sprintf_s(chServerString, "%d.%d.%d.%d:%d\n", gameServer.dwServerIP & 0xff, (gameServer.dwServerIP >> 8) & 0xff, (gameServer.dwServerIP >> 16) & 0xff, (gameServer.dwServerIP >> 24) & 0xff, _byteswap_ushort(gameServer.dwServerPort));

			out.append(chServerString);

		}

	}

	return out;
}

auto HaloCEFrameNetParser::getRequesterIp() -> std::string {

	char chRequesterIp[64]{ 0 };

	sprintf_s(chRequesterIp, "%d.%d.%d.%d\n", this->m_requesterIP & 0xff, (this->m_requesterIP >> 8) & 0xff, (this->m_requesterIP >> 16) & 0xff, (this->m_requesterIP >> 24) & 0xff);

	return std::string(chRequesterIp);
}

auto HaloCEFrameNetParser::setRequesterIp(const char* chIpv4) -> void {

	int b1{ 0 }, b2{ 0 }, b3{ 0 }, b4{ 0 }, port{ 0 };

	sscanf_s(chIpv4, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);

	this->m_requesterIP = (b4 << 24) |
		(b3 << 16) |
		(b2 << 8) |
		b1;

	std::printf("[OK] The requester IP Address has been changed\n");

}

auto HaloCEFrameNetParser::addServer(const char* chIpv4, const char* chPort) -> void {

	//TODO

}

auto HaloCEFrameNetParser::deleteServer(const char* chIpv4, const char* chPort) -> void {

	//TODO

}

auto HaloCEFrameNetParser::getNewPayload() -> unsigned char* {

	//TODO

	return 0;
}

auto HaloCEFrameNetParser::getRawPayload() -> unsigned char* {

	//TODO

	return 0;
}

auto HaloCEFrameNetParser::writeNewPayload(std::string& path) -> bool {

	//TODO

	return false;
}

HaloCEFrameNetParser::~HaloCEFrameNetParser() {

	this->m_gameFlags.clear();

	delete this->m_rawBuffer;

}