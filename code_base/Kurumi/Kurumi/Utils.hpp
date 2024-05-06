namespace Utils {

    auto find_section(const char* chName) -> std::pair<uintptr_t, uintptr_t> {

        auto imgNtH = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<unsigned char*>(::GetModuleHandle(NULL)) + reinterpret_cast<PIMAGE_DOS_HEADER>(::GetModuleHandle(NULL))->e_lfanew);

        auto sectionHeader = IMAGE_FIRST_SECTION(imgNtH);

        auto textVa = 0, textSize = 0;

        for (auto i = 0; i < imgNtH->FileHeader.NumberOfSections; ++i)

            if (strcmp(reinterpret_cast<char*>(sectionHeader[i].Name), chName) == 0) {

                textVa = imgNtH->OptionalHeader.ImageBase + sectionHeader[i].VirtualAddress;
                textSize = sectionHeader[i].SizeOfRawData;

                break;
            }

        return std::make_pair(textVa, textSize);
    }

};