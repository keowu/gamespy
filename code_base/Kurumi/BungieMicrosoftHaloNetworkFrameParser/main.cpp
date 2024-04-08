/*
    (C) Keowu - 2024
*/
#include <iostream>
#include "HaloCEFrameNetParser.hh"

auto main() -> int {

    std::cout << "Hello World!\n";

    std::string path("C:\\Users\\joaov\\source\\repos\\Kurumi\\BungieMicrosoftHaloNetworkFrameParser\\gamePayloadblank.bin");

    auto haloFrames = new HaloCEFrameNetParser(path);

    std::cout << haloFrames->operator std::string() << "\n";

    std::cout << "Requester IP: " << haloFrames->getRequesterIp() << "\n";

    haloFrames->setRequesterIp("175.45.176.80");

    std::cout << "Requester IP: " << haloFrames->getRequesterIp() << "\n";

    haloFrames->addServer("74.91.124.137", "2302");

    haloFrames->addServer("10.100.125.11", "2302");

    haloFrames->addServer("95.90.45.53", "2301");

    std::cout << haloFrames->operator std::string() << "\n";

    haloFrames->deleteServer("74.91.124.137", "2302");

    //std::cout << haloFrames->operator std::string() << "\n";

    haloFrames->getNewPayload();

    std::string pathout("C:\\Users\\joaov\\source\\repos\\Kurumi\\BungieMicrosoftHaloNetworkFrameParser\\NewGamePayload.bin");

    haloFrames->writeNewPayload(pathout);

    haloFrames->~HaloCEFrameNetParser();

    return 0;
}