/*
	File: GameIPC.hh
	Author: Jo�o Vitor(@Keowu)
	Created: 02/06/2024
	Last Update: 07/07/2024

	Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
namespace GameIPC {

	const std::string IPC_NAME("\\\\.\\pipe\\KewGameloader");

	const std::size_t MAX_BUFFER{ 1024 };

	static HANDLE hIPC = INVALID_HANDLE_VALUE;

	inline auto InitPipe() -> void {

		GameIPC::hIPC = ::CreateNamedPipeA(

			IPC_NAME.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1,
			MAX_BUFFER,
			MAX_BUFFER,
			NMPWAIT_USE_DEFAULT_WAIT,
			nullptr
		);

		ConnectNamedPipe(GameIPC::hIPC, NULL);

	}

	inline auto ReadData(std::string& strBuffer) -> bool {

		if (GameIPC::hIPC == INVALID_HANDLE_VALUE) {

			std::cout << "[DBG]: " << "THE IPC HANDLE IS INVALID\n";

			return false;
		}

		char chBuffer[MAX_BUFFER]{ 0 };
		DWORD szReaded;

		auto isReaded = ::ReadFile(

			hIPC,
			chBuffer,
			sizeof(chBuffer),
			&szReaded,
			nullptr

		);

		if (isReaded)
			strBuffer.assign(chBuffer, szReaded);

		return isReaded;
	}

	inline auto WriteData(std::string& strBuffer) -> bool {

		DWORD dwByteWritten{ 0 };

		auto isWrited = ::WriteFile(

			hIPC,
			strBuffer.c_str(),
			static_cast<DWORD>(strBuffer.length()),
			&dwByteWritten,
			nullptr

		);


		return isWrited;
	}

	inline auto ClosePipe() -> void {

		::CloseHandle(

			hIPC

		);

	}

};