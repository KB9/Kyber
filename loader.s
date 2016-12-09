global loader                                   ; The entry symbol for ELF

extern kmain									; kmain is defined in kmain.c

MAGIC_NUMBER equ 0x1BADB002                     ; The magic number constant
FLAGS        equ 0x0                            ; Multiboot flags
CHECKSUM     equ -MAGIC_NUMBER                  ; Calculate the checksum
                                                ; (magic number + checksum + flags should equal 0)
KERNEL_STACK_SIZE equ 4096                      ; Size of stack in bytes

section .bss
align 4                                         ; Align at 4 bytes
kernel_stack:                                   ; Label points to beginning of memory
    resb KERNEL_STACK_SIZE                      ; Reserve stack for the kernel
                             
section .text									; Start of the text (code) section
align 4                                         ; The code must be 4 byte aligned
    dd MAGIC_NUMBER                             ; Write the magic number to the machine code,
    dd FLAGS                                    ; the flags
    dd CHECKSUM                                 ; and the checksum
    
loader:                                         ; The loader label (defined as entry point in linker script)
    mov eax, 0xCAFEBABE                         ; Place the number 0xCAFEBABE in the register eax
    ;mov esp, kernel_stack + KERNEL_STACK_SIZE
    call kmain
.loop:
    jmp .loop                                   ; Loop forever
