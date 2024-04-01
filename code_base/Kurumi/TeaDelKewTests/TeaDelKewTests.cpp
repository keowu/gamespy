/*
    (C) Keowu - 2024
*/
#include <iostream>
#include "TeaDelKewAlgo.hh"

int main() {

    std::cout << "Hello World!\n";

    uint32_t plaintext[2]{0x01234567, 0x89abcdef};
    uint32_t key[4]{ 0x00B0B0CA, 0x00B0B0CA, 0x00B0B0CA, 0x00B0B0CA };

    TeaDelKewAlgo::tea_del_kew_encrypt(plaintext, key);

    TeaDelKewAlgo::tea_del_kew_decrypt(plaintext, key);


    return 0;
}