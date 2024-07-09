/*
    File: TeaDelKewAlgo.cpp
    Author: João Vitor(@Keowu)
    Created: 16/03/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#include <iostream>
#include "TeaDelKewAlgo.hh"

int main( 

) {

    std::cout << "Hello World!\n";

    uint32_t plaintext[ 2 ] { 0x01234567, 0x89abcdef };
    uint32_t key[ 4 ] { 0x00B0B0CA, 0x00B0B0CA, 0x00B0B0CA, 0x00B0B0CA };

    TeaDelKewAlgo::tea_del_kew_encrypt( plaintext, key );
                                                       
    TeaDelKewAlgo::tea_del_kew_decrypt( plaintext, key );

    return 0;
}