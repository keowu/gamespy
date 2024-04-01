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

    std::cout << "Requester IP: " << haloFrames->getRequesterIp() << "\n";

    haloFrames->setRequesterIp("175.45.176.80");

    std::cout << "Requester IP: " << haloFrames->getRequesterIp() << "\n";

    haloFrames->~HaloCEFrameNetParser();

    return 0;
}