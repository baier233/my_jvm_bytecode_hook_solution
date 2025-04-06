
.CODE

jhook_get_r14_address PROC
    mov rax, r14
    ret
jhook_get_r14_address ENDP

jhook_get_rax_address PROC
    ret
jhook_get_rax_address ENDP

END