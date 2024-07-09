/*
    File: TeaDelKewAlgo.hh
    Author: João Vitor(@Keowu)
    Created: 16/03/2024
    Last Update: 07/07/2024

    Copyright (c) 2024. github.com/keowu/gamespy. All rights reserved.
*/
#ifndef TeaDelKewAlgo_
#define TeaDelKewAlgo_

namespace TeaDelKewAlgo {

    static uint32_t kew_box[ 12 ] = {

        0x77122545, 0x88998877, 0x9944DEAD, 0x10CAFEB4,
        0x45B0B0C4, 0x35DEADDE, 0x25C4C4C4, 0x85634897,
        0x56123456, 0x11454545, 0x12323232, 0x95959595

    };

    static auto tea_del_kew_encrypt( 
        
        uint32_t plaintext[ 2 ],
        uint32_t key[ 4 ]
    
    ) -> void {

        uint32_t v0 = ~plaintext[ 0 ], v1 = ~plaintext[ 1 ];

        uint32_t sum = 0, delta = 0x00B0B0C4;

        uint32_t k0 = key[ 0 ] ^ 0xDEADBEEF, k1 = key[ 1 ] ^ 0xDEADBEEF,
            k2 = key[ 2 ] ^ 0xDEADBEEF, k3 = key[ 3 ] ^ 0xDEADBEEF;

        for ( int i = 0; i < 2048; i++ ) {

            v1 ^= kew_box[ i % 12 ];
            v0 ^= kew_box[ i % 12 ];
            v1 ^= ( i * 0x44444444 ^ ~i );
            v0 ^= ( i * 0x44444444 ^ ~i );

        }

        for ( int i = 0; i < 2048; i++ ) {

            v0 += ~( ( v1 << 4 ^ v1 >> 5 ) + v1 ) ^ ( sum + k0 );

            sum += delta;

            v1 += ~( ( v0 << 4 ^ v0 >> 5 ) + v0 ) ^ ( sum + k1 );

        }

        plaintext[ 0 ] = v0;
        plaintext[ 1 ] = v1;
    }

    static auto tea_del_kew_decrypt(
        
        uint32_t ciphertext[ 2 ],
        uint32_t key[ 4 ]
    
    ) -> void {

        uint32_t v0 = ciphertext[ 0 ], v1 = ciphertext[ 1 ];

        uint32_t sum = 0x85862000, delta = 0x00B0B0C4;

        uint32_t k0 = key[ 0 ] ^ 0xDEADBEEF, k1 = key[ 1 ] ^ 0xDEADBEEF,
            k2 = key[ 2 ] ^ 0xDEADBEEF, k3 = key[ 3 ] ^ 0xDEADBEEF;

        for ( int i = 0; i < 2048; i++ ) {

            v1 -= ~( ( v0 << 4 ^ v0 >> 5 ) + v0 ) ^ ( sum + k1 );

            sum -= delta;

            v0 -= ~( ( v1 << 4 ^ v1 >> 5 ) + v1 ) ^ ( sum + k0 );

        }

        for ( int i = 0; i < 2048; i++ ) {

            v1 ^= kew_box[ i % 12 ];
            v0 ^= kew_box[ i % 12 ];
            v1 ^= ( i * 0x44444444 ^ ~i );
            v0 ^= ( i * 0x44444444 ^ ~i );

        }

        ciphertext[ 0 ] = ~v0;
        ciphertext[ 1 ] = ~v1;
    }

};
#endif