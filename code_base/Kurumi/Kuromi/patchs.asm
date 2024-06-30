.model flat

extern _g_gs2004Recv : DWORD
extern _g_socket_gs2004_return: DWORD
extern _g_goadecbody_gs2004_return: DWORD

extern _memcpy: proc

.data
	old_gamespy_cls_buffer dd ?
	new_gamespy_cls_buffer db 4096 dup(0)

    gamespysocket struct
	    connection_status_flag dd ?
	    gap db 112 dup(?)
	    pGamespyBuffer dd ?
	    actual_size dd ?
	    gap1 db 1028 dup(?)
	    throw_gamespy_connection_error dd ?
	    state dd ?
	    gap2 db 24 dup(?)
	    socket dd ?
	    gap3 db 276 dup(?)
	    gs_state dd ?
    gamespysocket ends

    config_gamespy struct
	    state dd ?
	    gap db 4 dup(?)
	    keylist dd ?
	    gap1 db 104 dup(?)
	    pDecryptedBuffer dd ?
	    pDecryptBufferIn dd ?
	    values_to_populate db 1020 dup(?)
	    num_populate dd ?
	    expectedelements dd ?
	    ListCallBack dd ?
	    instance dd ?
	    gap2 db 8 dup(?)
	    ip_usuario_placed_requested dd ?
	    gap3 db 4 dup(?)
	    defaultPortGameservers dw ?
	    callback_body_init dd ?
	    gap4 db 12 dup(?)
	    crypt_key db ?
	    gap5 db 263 dup(?)
	    gs_query_flags db ?
	    pstate dd ?
    config_gamespy ends

.code

_new_get_socket_gamespy_buffer_gs2004_stub_bungie proc

	; Acessando os fields da struct. ESI contém a base para a struct "clsGSCon".
    mov edx, dword ptr [esi].gamespysocket.pGamespyBuffer
    mov eax, dword ptr [esi].gamespysocket.socket
         
	push edi ; Salvando valor antigo de EDI (OLD actual_size)

    mov edi, dword ptr [esi].gamespysocket.actual_size
         
	push 0          ; flags para serem utilizadas na chamada de recv
         
	mov  ecx, 1000h ; Tamanho máximo a ser lido do buffer, nesse caso -> 4096. 
	sub  ecx, edi   ; Subtraindo do tamanho máximo o tamanho já lido anteriormente, se houver. evitando estouro
	push ecx        ; Tamanho final a ser lido nesta interação

	; Obtendo o buffer original e substituindo por um nosso
	lea edx, new_gamespy_cls_buffer

	add  edx, edi   ; Avançando no buffer com o índice necessário
	push edx        ; Buffer a ser lido
         
	push eax        ; Socket Struct

	; Returning to original handler
	push _g_socket_gs2004_return
	ret
_new_get_socket_gamespy_buffer_gs2004_stub_bungie endp

_new_get_socket_gamespy_buffer_gs2004_stub_ea proc
    
        mov ecx, dword ptr [esi].gamespysocket.pGamespyBuffer
    	
        mov edx, dword ptr [esi].gamespysocket.socket
         
    	push edi

        mov edi, dword ptr [esi].gamespysocket.actual_size
         
    	push 0               ; flags
         
    	mov  eax, 1000h
    	sub  eax, edi
    	push eax             ; len

    	lea ecx, new_gamespy_cls_buffer

    	add  ecx, edi
    	push ecx             ; buf
         
    	push edx             ; s

    	call _g_gs2004Recv

    	; GS2004 V2 Buffer handling swap
    	pushad
    	pushfd
    
    	lea ecx, new_gamespy_cls_buffer
    	
        mov edx, dword ptr [esi].gamespysocket.pGamespyBuffer

    	push 4096
    	push ecx
    	push edx
    	call _memcpy
    
    	add esp, 0Ch

    	popfd
    	popad

    	; Returning to original handler
    	push _g_socket_gs2004_return
    	ret

_new_get_socket_gamespy_buffer_gs2004_stub_ea endp

_new_goa_decrypt_buffer_gs2004_stub_bungie proc

	sub  ebp, eax
    
    	lea edi, new_gamespy_cls_buffer

	push ebp

    lea eax, dword ptr [esi].config_gamespy.crypt_key

	push edi

	push eax

    mov dword ptr [esi].config_gamespy.pstate, 1
    
    	push _g_goadecbody_gs2004_return
	ret
_new_goa_decrypt_buffer_gs2004_stub_bungie endp


_new_goa_decrypt_buffer_gs2004_stub_ea proc

    	lea  edi, new_gamespy_cls_buffer
    	add  edi, eax ; Recalculate offset
    	sub  ebp, eax ; Recalculate offset
         
    	push ebp ;size

        lea ecx, dword ptr [esi].config_gamespy.crypt_key
         
    	mov  edx, edi ; 1º Argument

        mov dword ptr [esi].config_gamespy.pstate, 1

    	; GS 2004 V2, NEED TO ALIGN STACK because we are not using GOA
    	add esp, 4
    
    	push _g_goadecbody_gs2004_return
	ret
_new_goa_decrypt_buffer_gs2004_stub_ea endp


GoaGS2004Decrypt proc near

    	buffer1   = dword ptr  4
    	buffer2   = dword ptr  8
    	arg_stack = dword ptr  0Ch

    	push    ebx
    	mov     ebx, [esp+4+arg_stack]
    	push    esi
    	xor     esi, esi
    	test    ebx, ebx
    	jle     short loc_CheckBufferLength
    	push    ebp
    	mov     ebp, [esp+0Ch+buffer1]
    	push    edi
    	mov     edi, [esp+10h+buffer2]

loc_DecryptLoop:
    	xor     eax, eax
    	mov     al, [esi+edi]
    	push    eax
    	push    ebp
    	call    GoaExecuteDecrypt
    	add     esp, 8
    	mov     [esi+edi], al
    	inc     esi
    	cmp     esi, ebx
    	jl      short loc_DecryptLoop
    	pop     edi
    	pop     ebp

loc_CheckBufferLength:
    	pop     esi
    	pop     ebx

    	retn
GoaGS2004Decrypt      endp


GoaExecuteDecrypt proc near

    	arg_0           = dword ptr  4
    	byte_encrypt    = byte ptr  8

    	mov     eax, [esp+arg_0]
    	mov     cl, [eax+100h]
    	movzx   edx, cl
    	mov     dl, [edx+eax]
    	push    ebx
    	mov     bl, [eax+101h]
    	add     bl, dl
    	mov     [eax+101h], bl
    	inc     cl
    	mov     [eax+100h], cl
    	movzx   ecx, byte ptr [eax+104h]
    	mov     dl, [ecx+eax]
    	add     ecx, eax
    	push    esi
    	movzx   esi, bl
    	mov     bl, [esi+eax]
    	mov     [ecx], bl
    	movzx   ecx, byte ptr [eax+103h]
    	mov     cl, [ecx+eax]
    	movzx   esi, byte ptr [eax+101h]
    	mov     [esi+eax], cl
    	movzx   ecx, byte ptr [eax+100h]
    	mov     cl, [ecx+eax]
    	movzx   esi, byte ptr [eax+103h]
    	mov     [esi+eax], cl
    	movzx   ecx, byte ptr [eax+100h]
    	mov     [ecx+eax], dl
    	mov     bl, [eax+102h]
    	movzx   esi, byte ptr [eax+101h]
    	movzx   edx, dl
    	mov     cl, [edx+eax]
    	movzx   edx, byte ptr [eax+103h]
    	add     bl, cl
    	mov     [eax+102h], bl
    	movzx   edx, byte ptr [edx+eax]
    	mov     cl, bl
    	xor     ebx, ebx
    	mov     bl, [esi+eax]
    	movzx   esi, byte ptr [eax+104h]
    	movzx   ecx, cl
    	movzx   ecx, byte ptr [ecx+eax]
    	add     edx, ebx
    	xor     ebx, ebx
    	mov     bl, [esi+eax]
    	movzx   esi, byte ptr [eax+100h]
    	add     edx, ebx
    	xor     ebx, ebx
    	mov     bl, [esi+eax]
    	and     edx, 0FFh
    	movzx   edx, byte ptr [edx+eax]
    	mov     dl, [edx+eax]
    	pop     esi
    	add     ecx, ebx
    	and     ecx, 0FFh
    	mov     bl, [ecx+eax]
    	mov     cl, [esp+4+byte_encrypt]
    	xor     dl, bl
    	xor     dl, cl
    	mov     [eax+103h], dl
    	mov     [eax+104h], cl
    	mov     al, dl
    	pop     ebx
    	retn
GoaExecuteDecrypt endp

end