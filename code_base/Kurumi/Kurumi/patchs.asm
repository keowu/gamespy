;
;    (C) Keowu - 2024
;
.model flat

includelib ucrt.lib
includelib legacy_stdio_definitions.lib
extern _memcpy: proc

.data

	; New buffer data
	patch_first_byte db ?
	patch_frame_buffer db 7FFh dup(0)

	; Original buffer to replace things with a fake buffer when read a decrypted buffer on new buffer data
	old_first_byte_addr dd 9798B0h
	old_frame_buffer dd 9798B1h

	; Backup buffer data
	bckp_first_byte     db 0E4h
	bckp_frame_buffer   db 0B1h, 0C7h, 0CAh, 67h, 48h, 9Ah, 6Bh, 5Ah, 0E5h, 64h
						db 0FFh, 26h, 0F3h, 20h, 45h, 0, 10h, 0F1h, 0A2h, 0C3h
						db 9Eh, 82h, 99h, 0D3h, 0C9h, 91h, 97h, 0DAh, 90h, 2Dh
						db 0B0h, 0Ch, 44h, 0, 20h, 64h, 0DEh, 0D3h, 0E5h, 4Bh
						db 0AAh, 0Ah, 0D3h, 0C0h, 7Ch, 0F0h, 0FBh, 48h, 0AFh, 0ABh
						db 0Fh, 0, 30h, 0A7h, 0F5h, 3Dh, 0B1h, 4Fh, 0B5h, 0D7h
						db 7Ah, 89h, 0CCh, 0E6h, 4Fh, 95h, 0BDh, 9Eh, 3, 0, 40h
						db 0C2h, 0EDh, 0B6h, 42h, 0CFh, 0BBh, 9Ah, 0CDh, 17h, 45h
						db 0Ah, 3Dh, 7Fh, 91h, 0FCh, 44h, 0, 50h, 0Ch, 2Ah, 0ABh
						db 0B8h, 2Ah, 4Eh, 3Fh, 94h, 0E6h, 0F2h, 64h, 0, 0DDh
						db 62h, 0F1h, 49h, 0, 60h, 0A9h, 7, 7Bh, 0DFh, 36h, 5Dh
						db 0D6h, 11h, 6Eh, 0D5h, 0CFh, 0BAh, 0BDh, 3Ah, 10h, 4Dh
						db 0, 70h, 0A2h, 0Bh, 9Fh, 0A4h, 39h, 61h, 54h, 27h, 0E1h
						db 54h, 41h, 61h, 0C4h, 0AAh, 45h, 7Dh, 0, 80h, 96h, 0D8h
						db 4Bh, 6Dh, 0EAh, 9, 14h, 0CBh, 0AFh, 9Ah, 0C0h, 0B7h
						db 67h, 0A1h, 0A5h, 46h, 0, 90h, 0E8h, 2Fh, 0Eh, 0CEh
						db 84h, 0F2h, 0D0h, 9Fh, 0A3h, 6Ch, 3Ch, 0A6h, 71h, 3Ah
						db 0B0h, 9Ch, 0, 0A0h, 74h, 0, 0AEh, 68h, 0EEh, 0DAh, 0ECh
						db 74h, 0A0h, 0FBh, 0A6h, 0A3h, 16h, 84h, 71h, 42h, 0
						db 0B0h, 0B4h, 0D3h, 5Fh, 6Ah, 88h, 64h, 8, 7Eh, 0CDh
						db 60h, 5Ch, 1Eh, 4Ah, 0Ch, 72h, 0E1h, 0, 0C0h, 68h, 0F4h
						db 1, 0E6h, 3Bh, 97h, 0A6h, 0F3h, 0CEh, 0C4h, 40h, 0C1h
						db 0AAh, 2Bh, 0A0h, 94h, 0, 0D0h, 13h, 9Fh, 0E3h, 0D8h
						db 49h, 21h, 0FDh, 0C7h, 8Eh, 37h, 8Bh, 0EAh, 4Ch, 78h
						db 0A9h, 0DDh, 0, 0E0h, 0F5h, 8, 0BEh, 2Bh, 47h, 62h, 0E3h
						db 0AFh, 0AAh, 9Eh, 0D4h, 98h, 83h, 3Eh, 6Ah, 0CFh, 0
						db 0F0h, 9Dh, 0EBh, 0FAh, 0E8h, 13h, 0CFh, 6Bh, 0Eh, 6Ch
						db 8Fh, 0E3h, 0C2h, 16h, 85h, 59h, 0Fh, 1, 0, 2Dh, 0D8h
						db 8Ch, 65h, 1Ah, 7Dh, 9Fh, 62h, 78h, 0C3h, 1, 0F5h, 0Ah
						db 4Eh, 33h, 8Bh, 1, 10h, 7Fh, 0C2h, 7, 0C6h, 0Fh, 7Ah
						db 0F9h, 4Ch, 84h, 46h, 14h, 8Fh, 95h, 93h, 23h, 40h, 1
						db 20h, 0C7h, 7Bh, 53h, 0Bh, 0DEh, 6Bh, 4, 0Dh, 0BBh, 57h
						db 2, 30h, 0F6h, 7, 3Fh, 85h, 1, 30h, 7Ch, 0B7h, 0E5h
						db 94h, 0D0h, 64h, 2Bh, 0F4h, 0DBh, 0D3h, 63h, 33h, 73h
						db 38h, 9, 0DAh, 1, 40h, 4Eh, 0AFh, 72h, 0D3h, 0A2h, 4Ah
						db 0A8h, 7Ah, 4Ch, 3Dh, 60h, 85h, 0ABh, 47h, 87h, 69h
						db 1, 50h, 0B5h, 0ECh, 0C2h, 2Ah, 2Dh, 0CFh, 0E1h, 2Bh
						db 5Ch, 0F9h, 51h, 0DFh, 48h, 6Dh, 1, 0, 1, 60h, 0D4h
						db 4Eh, 0EAh, 81h, 4Fh, 0BBh, 3Ch, 0D1h, 1Dh, 0EBh, 0E8h
						db 0DFh, 4Ah, 0A2h, 0C3h, 0BEh, 1, 70h, 44h, 0D2h, 28h
						db 8Eh, 0A4h, 30h, 90h, 0DCh, 0D3h, 0E0h, 58h, 0DEh, 0C6h
						db 66h, 4Fh, 90h, 1, 80h, 8Bh, 0EDh, 9Eh, 0F1h, 83h, 0B9h
						db 7Ah, 58h, 1Dh, 88h, 0E2h, 0A5h, 0FCh, 6Dh, 1Eh, 5, 1
						db 90h, 16h, 9Bh, 90h, 51h, 0D4h, 0EBh, 0E4h, 0E8h, 0F8h
						db 7Ah, 69h, 14h, 45h, 0A2h, 42h, 0DDh, 1, 0A0h, 89h, 0F5h
						db 9Ch, 12h, 3Dh, 7Eh, 7Ch, 70h, 13h, 0F3h, 0C6h, 80h
						db 0FBh, 0CAh, 0EDh, 58h, 1, 0B0h, 1Dh, 0D7h, 0B3h, 10h
						db 71h, 0A1h, 47h, 4Dh, 39h, 0C9h, 3Ah, 0B5h, 0D8h, 2Ch
						db 3Ch, 3, 1, 0C0h, 47h, 35h, 2Fh, 67h, 6Bh, 79h, 0FFh
						db 2Bh, 8Dh, 8Dh, 0D2h, 44h, 8Dh, 39h, 0E4h, 88h, 1, 0D0h
						db 40h, 76h, 1, 56h, 0B0h, 1Ch, 10h, 0A0h, 32h, 87h, 20h
						db 2Bh, 1Eh, 6Bh, 77h, 11h, 1, 0E0h, 0C1h, 0FBh, 0D8h
						db 77h, 0EAh, 0, 5Eh, 0DEh, 4Fh, 0C9h, 0CDh, 0B6h, 0ECh
						db 4Fh, 0B2h, 6Eh, 1, 0F0h, 2Eh, 33h, 0CEh, 0D0h, 71h
						db 0BCh, 0BEh, 77h, 24h, 38h, 88h, 49h, 9Ah, 0C1h, 31h
						db 0A1h, 2, 0, 32h, 0B5h, 76h, 0C9h, 10h, 0C6h, 0F8h, 43h
						db 0C2h, 0BCh, 2Ch, 0FAh, 68h, 0Fh, 0D1h, 0FBh, 2, 10h
						db 10h, 0C1h, 21h, 0F8h, 69h, 0A0h, 63h, 0D9h, 42h, 0A0h
						db 7Bh, 98h, 0CEh, 0ECh, 5Bh, 0Ch, 2, 20h, 0C2h, 48h, 27h
						db 48h, 3Dh, 0CFh, 0BEh, 8Bh, 35h, 0FEh, 69h, 43h, 0ECh
						db 0C1h, 0D4h, 26h, 2, 30h, 0C9h, 45h, 4Dh, 7Fh, 3, 11h
						db 0BEh, 0E4h, 0A2h, 8Eh, 70h, 75h, 3Eh, 84h, 0C7h, 0ECh
						db 2, 40h, 0Bh, 6Ah, 0A6h, 88h, 5Eh, 42h, 62h, 0F7h, 18h
						db 0AAh, 50h, 90h, 2Dh, 0C1h, 3, 89h, 2, 50h, 1Ch, 1Eh
						db 9, 9Bh, 67h, 19h, 48h, 20h, 32h, 23h, 0BDh, 0A7h, 2Ah
						db 74h, 0B5h, 73h, 2, 60h, 22h, 28h, 5Ch, 67h, 61h, 6Dh
						db 65h, 6Eh, 61h, 6Dh, 65h, 5Ch, 62h, 66h, 69h, 65h, 6Ch
						db 64h, 31h, 39h, 34h, 32h, 5Ch, 67h, 61h, 6Dh, 65h, 76h
						db 65h, 72h, 5Ch, 32h, 5Ch, 6Ch, 6Fh, 63h, 61h, 74h, 69h
						db 6Fh, 6Eh, 5Ch, 30h, 5Ch, 76h, 61h, 6Ch, 69h, 64h, 61h
						db 74h, 65h, 5Ch, 32h, 65h, 6Eh, 4Fh, 36h, 36h, 42h, 64h
						db 5Ch, 65h, 6Eh, 63h, 74h, 79h, 70h, 65h, 5Ch, 32h, 5Ch
						db 66h, 69h, 6Eh, 61h, 6Ch, 5Ch, 5Ch, 71h, 75h, 65h, 72h
						db 79h, 69h, 64h, 5Ch, 31h, 2Eh, 31h, 5Ch, 5Ch, 6Ch, 69h
						db 73h, 74h, 5Ch, 63h, 6Dh, 70h, 5Ch, 67h, 61h, 6Dh, 65h
						db 6Eh, 61h, 6Dh, 65h, 5Ch, 62h, 66h, 69h, 65h, 6Ch, 64h
						db 31h, 39h, 34h, 32h, 5Ch, 66h, 69h, 6Eh, 61h, 6Ch, 5Ch
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
						db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

		read_buffer_gs_return dd ?
		decrypt_routine_gs_return dd ?

.code

	_set_first_magic_byte_addr proc

		mov eax, dword ptr [esp+4]
		mov old_first_byte_addr, eax  ; Stack argument One
		inc eax ; Increment 1 to find the frame_buffer pointer that is 1 magic + frame_buffer
		mov old_frame_buffer, eax ; Mov frame buffer pointer to global.

		ret
	_set_first_magic_byte_addr endp

	_set_decrypt_routine_gs_return proc

		mov eax, dword ptr [esp+4]
		mov decrypt_routine_gs_return, eax  ; Stack argument One

		ret
	_set_decrypt_routine_gs_return endp

	_set_read_buffer_gs_return proc
		
		mov eax, dword ptr [esp+4]
		mov read_buffer_gs_return, eax  ; Stack argument One

		ret
	_set_read_buffer_gs_return endp

	_fake_frames_to_decrypt proc

		pushad
		pushfd

		xor eax, eax ; eax = 0
		xor ebx, ebx ; ebx = 0
		mov ebx,  dword ptr [old_first_byte_addr]
		mov al, byte ptr [bckp_first_byte]
		mov byte ptr [ebx], al

		mov ecx, 0
		
		lea edx, bckp_frame_buffer
		
		push 7FFh
		push edx
		push old_frame_buffer
		call _memcpy

		; Adjust stack back to normal
		add esp, 0Ch

		; restauring registers context
		popfd
		popad
		ret
	_fake_frames_to_decrypt endp

	_replaced_read_buffer proc

		call _fake_frames_to_decrypt ; Modify the frames into a new GS buffer handler

		mov eax, dword ptr [patch_first_byte]
		push ebx
		push edi
		mov ecx,7FFh
		sub ecx, eax
		push ecx ; len
		lea  edx, patch_frame_buffer[eax]
		mov eax, [esi+88h]
		push edx             ; bufffer
		push eax             ; socket struct

		push read_buffer_gs_return ; return to recv

		ret
	_replaced_read_buffer endp

GameSpyDecrompressSection segment

	_fake_gamespy_decompress_routine_2 proc

		sub eax, edi
		add eax, 09798B0h
		push eax
		mov edx, edi
		mov ecx, ebp
		call _decompress_two ; Call my own decompress2 assembly implementation

		pushad
		pushfd

		; Replace gamespy buffer to the readed one from us gameserver emulator
		lea edx, patch_frame_buffer
		push 7FFh
		push edx
		push old_frame_buffer
		call _memcpy

		; adjust stack back to normal
		add esp, 0Ch
		
		popfd
		popad
		
		add edi, 5 ; ALIGN THE GAMESPY STRUCT. hehe the nandayo

		push decrypt_routine_gs_return
		ret

	_fake_gamespy_decompress_routine_2 endp


	_decompress      proc near

		localvar_1 = dword ptr -4
		argumentos = dword ptr  4

                push    ecx
                mov     eax, [esp+4+argumentos]
                push    ebx
                mov     ebx, [ecx+4CCh]
                push    ebp
                mov     ebp, edx
                mov     edx, [ecx+4C0h]
                push    esi
                mov     esi, [ecx+4C8h]
                push    edi
                lea     edi, [ebp+eax*4+0]
                cmp     ebp, edi
                mov     eax, [ecx+4C4h]
                mov     [esp+14h+localvar_1], ebp
                mov     [esp+14h+argumentos], edi
                jnb     clean_and_copy_decompress_result

continue_decompression:
                cmp     ebx, 10000h
                jnb     short exceded_max_block_size_go_to_next
                lea     ebp, [ecx+edx*4+440h]

block_size_not_reached_continue_decompression:
                add     eax, ebx
                add     esi, eax
                add     eax, esi
                mov     [ebp+0], esi
                mov     edi, esi
                shr     esi, 8
                shl     edi, 18h
                or      edi, esi
                mov     [ebp-40h], eax
                mov     esi, eax
                and     esi, 0FFh
                mov     [ebp+40h], ebx
                xor     eax, [ecx+esi*4]
                mov     esi, edi
                and     esi, 0FFh
                xor     edi, [ecx+esi*4]
                add     ebx, ebx
                mov     esi, edi
                shr     edi, 8
                shl     esi, 18h
                or      esi, edi
                mov     edi, eax
                shl     eax, 8
                shr     edi, 18h
                or      edi, eax
                mov     eax, edi
                and     eax, 0FFh
                xor     edi, [ecx+eax*4]
                mov     eax, esi
                and     eax, 0FFh
                xor     esi, [ecx+eax*4]
                mov     eax, edi
                shr     eax, 18h
                shl     edi, 8
                inc     edx
                add     ebp, 4
                or      eax, edi
                cmp     ebx, 10000h
                jb      short block_size_not_reached_continue_decompression

exceded_max_block_size_go_to_next:
                xor     esi, eax
                mov     eax, [esp+14h+localvar_1]
                mov     [eax], esi
                add     eax, 4
                dec     edx
                mov     [esp+14h+localvar_1], eax
                jns     short advance_next_block
                xor     edx, edx

advance_next_block:
                mov     eax, [ecx+edx*4+400h]
                mov     ebx, [ecx+edx*4+440h]
                not     eax
                mov     esi, eax
                shl     esi, 18h
                shr     eax, 8
                or      esi, eax
                mov     eax, esi
                and     eax, 0FFh
                xor     esi, [ecx+eax*4]
                mov     eax, ebx
                and     eax, 0FFh
                mov     ebp, [ecx+eax*4]
                mov     eax, esi
                shr     esi, 8
                xor     ebx, ebp
                shl     eax, 18h
                or      eax, esi
                mov     edi, ebx
                shr     edi, 18h
                shl     ebx, 8
                or      edi, ebx
                mov     ebx, [ecx+edx*4+480h]
                mov     esi, eax
                and     esi, 0FFh
                mov     ebp, [ecx+esi*4]
                mov     esi, edi
                and     esi, 0FFh
                xor     eax, ebp
                xor     edi, [ecx+esi*4]
                mov     ebp, [esp+14h+localvar_1]
                mov     esi, edi
                shl     edi, 8
                shr     esi, 18h
                or      esi, edi
                mov     edi, [esp+14h+argumentos]
                cmp     ebp, edi
                lea     ebx, [ebx+ebx+1]
                jb      continue_decompression

clean_and_copy_decompress_result:
                pop     edi
                mov     [ecx+4C8h], esi
                pop     esi
                pop     ebp
                mov     [ecx+4CCh], ebx
                mov     [ecx+4C0h], edx
                mov     [ecx+4C4h], eax
                pop     ebx
                pop     ecx
                retn    4
_decompress      endp

_decompress_two  proc near

data_size       = dword ptr  4

                push    ebx
                push    ebp
                mov     ebp, [esp+8+data_size]
                push    esi
                push    edi
                xor     edi, edi
                test    ebp, ebp
                mov     ebx, edx
                mov     esi, ecx
                jle     short clean_and_exit

loop_head_compressed_codeblock:
                mov     eax, [esi+514h]
                test    eax, eax
                jz      short init_block_decompression
                sub     eax, esi
                sub     eax, 4D4h
                cmp     eax, 3Fh
                jb      short check_block_already_decompressed

init_block_decompression:
                lea     edx, [esi+4D4h]
                push    10h
                mov     ecx, esi
                mov     [esi+514h], edx
                call    _decompress

check_block_already_decompressed:
                mov     eax, [esi+514h]
                mov     cl, [eax]
                xor     [edi+ebx], cl
                mov     edx, [esi+514h]
                inc     edx
                inc     edi
                cmp     edi, ebp
                mov     [esi+514h], edx
                jl      short loop_head_compressed_codeblock

clean_and_exit:
                pop     edi
                pop     esi
                pop     ebp
                pop     ebx
                retn    4
_decompress_two  endp

GameSpyDecrompressSection ends

end