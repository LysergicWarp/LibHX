/* The memory a program is linked into.
 *
 * The file the BIOS loads holds the code, the constants and every initial value. It is loaded
 * to Work RAM-H and entered at its first byte, which is startup's. Startup copies the initial
 * values that belong elsewhere to their places.
 *
 * The link's settings come ahead of this, from the program's build:
 *   hx_load         where the BIOS loads the file: the 1st Read Address
 *   hx_stack_low    0 for a stack at the top of Work RAM-H, another value for one below the
 *                   program
 *   hx_stack_size   the stack's size at the top of Work RAM-H
 *
 * A name here has no _ ahead of it unless C is to reach it: C puts one ahead of its own. */

OUTPUT_FORMAT("elf32-sh")
ENTRY(_hx_start)

hx_low_ram_start = 0x00200000;
hx_low_ram_end = 0x00300000;
hx_cart_start = 0x02400000;
hx_cart_end = 0x02800000;
hx_high_ram_end = 0x06100000;

/* Work RAM-H below this is the BIOS's own, and above that the second SH-2's stack when the
 * disc's header names none */
hx_low_stack_start = 0x06001000;

/* The Initial Program is loaded here, and is this long on a disc of the SDK's */
hx_ip_end = 0x06002000 + 0x1000;

/* The stack at the top of Work RAM-H is taken off the program's memory there. The stack below
 * the program runs from the program's first byte down, over the Initial Program, which has
 * run by then */
hx_stack_start = hx_stack_low ? hx_low_stack_start : hx_high_ram_end - hx_stack_size;
hx_stack_end = hx_stack_low ? hx_load : hx_high_ram_end;
hx_program_end = hx_stack_low ? hx_high_ram_end : hx_stack_start;

ASSERT(hx_load % 4 == 0, "hx_load is no multiple of 4")
ASSERT(hx_load > hx_ip_end && hx_load < hx_high_ram_end - 4,
    "hx_load is not between the Initial Program and the end of Work RAM-H")
ASSERT(hx_stack_size % 4 == 0, "hx_stack_size is no multiple of 4")
ASSERT(hx_stack_end > hx_stack_start, "the stack has no size")
ASSERT(hx_program_end > hx_load, "hx_stack_size leaves the program no memory")

MEMORY
{
        low_ram (rw) : ORIGIN = hx_low_ram_start, LENGTH = hx_low_ram_end - hx_low_ram_start
        cart (rw) : ORIGIN = hx_cart_start, LENGTH = hx_cart_end - hx_cart_start
        high_ram (rwx) : ORIGIN = hx_load, LENGTH = hx_program_end - hx_load
}

SECTIONS
{
        .text : {
                KEEP(*(.text.hx_start))
                *(.text .text.*)
                . = ALIGN(4);
        } > high_ram

        .rodata : {
                *(.rodata .rodata.*)
                . = ALIGN(4);
        } > high_ram

        /* Ahead of .data and .bss, whose patterns would take these sections too */
        .low_ram.data : {
                hx_low_ram_data = .;
                *(.data.hx_low_ram.*)
                . = ALIGN(4);
                hx_low_ram_data_end = .;
        } > low_ram AT> high_ram

        .low_ram.bss (NOLOAD) : {
                hx_low_ram_bss = .;
                *(.bss.hx_low_ram.*)
                . = ALIGN(4);
                hx_low_ram_bss_end = .;
        } > low_ram AT> low_ram

        .cart.data : {
                *(.data.hx_cart.*)
                . = ALIGN(4);
        } > cart AT> high_ram

        .cart.bss (NOLOAD) : {
                *(.bss.hx_cart.*)
                . = ALIGN(4);
        } > cart AT> cart

        .data : {
                *(.data .data.*)
                . = ALIGN(4);
        } > high_ram

        .bss (NOLOAD) : {
                hx_bss = .;
                *(.bss .bss.*)
                *(COMMON)
                . = ALIGN(4);
                hx_bss_end = .;
        } > high_ram

        /* Startup runs no constructor yet. Kept, though nothing refers to them, so the check
         * below sees them */
        .init : {
                KEEP(*(.init_array .init_array.* .ctors .ctors.*))
                KEEP(*(.fini_array .fini_array.* .dtors .dtors.*))
        } > high_ram

        /* No code here unwinds the stack */
        /DISCARD/ : {
                *(.eh_frame .eh_frame.*)
        }

        /* The objects' relocations, which the link applies and leaves out */
        .rela.dyn : { *(.rela.*) }

        .comment 0 : { *(.comment) }
        .debug_abbrev 0 : { *(.debug_abbrev) }
        .debug_addr 0 : { *(.debug_addr) }
        .debug_aranges 0 : { *(.debug_aranges) }
        .debug_frame 0 : { *(.debug_frame) }
        .debug_info 0 : { *(.debug_info) }
        .debug_line 0 : { *(.debug_line) }
        .debug_gnu_pubnames 0 : { *(.debug_gnu_pubnames) }
        .debug_gnu_pubtypes 0 : { *(.debug_gnu_pubtypes) }
        .debug_line_str 0 : { *(.debug_line_str) }
        .debug_loc 0 : { *(.debug_loc) }
        .debug_loclists 0 : { *(.debug_loclists) }
        .debug_macinfo 0 : { *(.debug_macinfo) }
        .debug_macro 0 : { *(.debug_macro) }
        .debug_names 0 : { *(.debug_names) }
        .debug_pubnames 0 : { *(.debug_pubnames) }
        .debug_pubtypes 0 : { *(.debug_pubtypes) }
        .debug_ranges 0 : { *(.debug_ranges) }
        .debug_rnglists 0 : { *(.debug_rnglists) }
        .debug_str 0 : { *(.debug_str) }
        .debug_str_offsets 0 : { *(.debug_str_offsets) }
        .debug_types 0 : { *(.debug_types) }
}

hx_low_ram_data_load = LOADADDR(.low_ram.data);

/* For C: the stack, and what each region has left past the program's own */
_hx_stack_start = hx_stack_start;
_hx_stack_end = hx_stack_end;
_hx_high_ram_free = hx_bss_end;
_hx_high_ram_end = hx_program_end;
_hx_low_ram_free = hx_low_ram_bss_end;
_hx_low_ram_end = hx_low_ram_end;
_hx_cart_free = ADDR(.cart.bss) + SIZEOF(.cart.bss);
_hx_cart_end = hx_cart_end;

ASSERT(_hx_start == hx_load, "startup is not the program's first byte")
ASSERT(SIZEOF(.init) == 0,
    "the program has a constructor or a destructor for startup to run, which it does not do")
ASSERT(SIZEOF(.cart.data) == 0 && SIZEOF(.cart.bss) == 0,
    "the program places data in the cartridge, which startup does not set up")
