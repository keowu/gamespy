/*
    File: main.cpp
    Author: João Vitor(@Keowu)
    Created: 16/03/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include <iostream>
#include "BF1942FrameNetParser.hh"
#include "BFVietnamFrameNetParser.hh"


auto Bf1942Work(

) -> void {

    std::cout << "Hello World!\n";

    std::string path( "payload.bin" );

    auto bf1942 = new BF1942FrameNetParser( path );

    std::cout << bf1942->operator std::string( ) << "\n";

    bf1942->addNewServer( "184.71.170.154", "23000" );
    bf1942->addNewServer( "176.9.19.239", "23000" );
    bf1942->addNewServer( "51.81.48.224", "23000" );
    bf1942->addNewServer( "37.187.92.16", "23000" );
    bf1942->addNewServer( "108.61.119.37", "23000" );
    bf1942->addNewServer( "37.187.92.162", "23000" );
    bf1942->addNewServer( "168.235.94.165", "23000" );
    bf1942->addNewServer( "37.187.92.162", "23000" );
    bf1942->addNewServer( "184.71.170.154", "23000" );

    std::cout << bf1942->operator std::string( ) << "\n";

    auto NewPayloadBytes = bf1942->getNewPayload( );

    bf1942->writeNewPayload( path );

    bf1942->~BF1942FrameNetParser( );

}

auto BfVietnamWork(

) -> void {

    std::string path( "payload3.bin" );

    auto bfv = new BFVietnamFrameNetParser( path );

    std::cout << bfv->operator std::string( ) << "\n";

    bfv->addServer( "178.254.31.241", "23000" );

    //bfv->writeNewPayload(path);

    //std::cout << bfv->operator std::string() << "\n";

    //bfv->~BFVietnamFrameNetParser();
}

auto main(

) -> int {

    BfVietnamWork( );

    return 0;
}