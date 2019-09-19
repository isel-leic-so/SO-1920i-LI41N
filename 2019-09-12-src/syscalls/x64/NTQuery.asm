; for Windows 10 version 1903

.code

NtQueryTimerResolution proc
 mov	r10, rcx
 mov	eax, 348  ; syscall number (0x15c)
 syscall
 ret 
NtQueryTimerResolution endp

end