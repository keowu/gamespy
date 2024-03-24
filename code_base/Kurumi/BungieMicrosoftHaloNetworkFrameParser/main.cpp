/*
    (C) Keowu - 2024
*/
#include <iostream>
#include "HaloCEFrameNetParser.hh"

auto main() -> int {

    std::cout << "Hello World!\n";

    std::string path("C:\\Users\\joaov\\source\\repos\\Kurumi\\BungieMicrosoftHaloNetworkFrameParser\\gamePayload.bin");

    auto haloFrames = new HaloCEFrameNetParser(path);

    std::cout << haloFrames->operator std::string() << "\n";

    return 0;
}