// src/dump.c
// Copyright (C) 2021 Ethan Uppal. All rights reserved.

#include "dump.h"
#include "safe.h"
#include "termcolor.h"
#define printf tcol_printf
#define fprintf tcol_fprintf
#include <mach-o/loader.h>
#include <mach-o/nlist.h>

#define S(...) struct __VA_ARGS__
#define START_READ() size_t __CUR = 0
#define READ(n) buffer + __CUR; do { \
    __CUR += (n); \
    if (__CUR >= length) return; \
} while (0)
#define CONSUME(n) __CUR += (n)

#define PRINT_FLAG(flags, flag) \
    if ((flags) & (flag)) printf(" {+}" #flag "{0}")
#define PRINT_FLAG_EXT(flags, flag, extra) \
    if ((flags) & (flag)) printf(" {+}" #flag "{0}" extra)
#define PRINT_OPTION(value, instance) \
    if ((value) == (instance)) printf("{+}" #instance "{0}\n")
#define PRINT_OPTION_EXT(value, instance, extra) \
    if ((value) == (instance)) printf("{+}" #instance "{0}" extra "\n")

#define local static inline

local const char* argz_get_string(const char* start, size_t index) {
    const char* next = start;
    while (index > 0) {
        while (*next++);
        start = next + 1;
        index--;
    };
    return start;
}

local void dump_header(void* buffer, S(mach_header_64)* header) {
    printf("│ {C}Header{0}: {M+}struct {0}mach_header_64\n");
    printf("└─┐ Magic: {Y}0x%08x{0}\n", header->magic);

    printf("  │ CPU Type: {Y}0x%08x{0}: ", header->cputype);
    PRINT_OPTION(header->cputype, CPU_TYPE_X86_64);
    PRINT_OPTION(header->cputype, CPU_TYPE_POWERPC64);
    PRINT_OPTION(header->cputype, CPU_TYPE_ANY);

    printf("  │ CPU Subtype: {Y}0x%08x{0}:", header->cpusubtype);
    PRINT_OPTION(header->cpusubtype, CPU_SUBTYPE_POWERPC_ALL);
    PRINT_OPTION(header->cpusubtype, CPU_SUBTYPE_X86_64_ALL);

    printf("  │ File Type: {Y}0x%08x{0}: ", header->filetype);
    PRINT_OPTION_EXT(header->filetype, MH_OBJECT,
                     ": Intermediate Object File"); else
    PRINT_OPTION_EXT(header->filetype, MH_EXECUTE,
                     ": Standard Executable Program"); else
    PRINT_OPTION_EXT(header->filetype, MH_BUNDLE,
                     ": Runtime Bundle"); else
    PRINT_OPTION_EXT(header->filetype, MH_DYLIB,
                     ": Dynamic Shared Library"); else
    PRINT_OPTION_EXT(header->filetype, MH_CORE,
                     ": Core File"); else
    PRINT_OPTION_EXT(header->filetype, MH_DYLINKER,
                     ": Dynamic Linker Shared Library"); else
    PRINT_OPTION_EXT(header->filetype, MH_DSYM,
                     ": Symbol Information File"); else
    PRINT_OPTION_EXT(header->filetype, MH_OBJECT,
                     ": Intermediate Object File"); else
    PRINT_OPTION_EXT(header->filetype, MH_OBJECT,
                     ": Intermediate Object File"); else {
        printf("Unknown file type\n");
    }

    printf("  │ Number of load commands: %u\n", header->ncmds);
    printf("  │ Size of load commands: %u byte(s)\n", header->sizeofcmds);

    printf("┌─┘ Flags: {Y}0x%08x{0}:", header->flags);
    PRINT_FLAG(header->flags, MH_NOUNDEFS);
    PRINT_FLAG(header->flags, MH_INCRLINK);
    PRINT_FLAG(header->flags, MH_DYLDLINK);
    PRINT_FLAG(header->flags, MH_TWOLEVEL);
    PRINT_FLAG(header->flags, MH_BINDATLOAD);
    PRINT_FLAG(header->flags, MH_PREBOUND);
    PRINT_FLAG(header->flags, MH_PREBINDABLE);
    PRINT_FLAG(header->flags, MH_NOFIXPREBINDING);
    PRINT_FLAG(header->flags, MH_ALLMODSBOUND);
    PRINT_FLAG(header->flags, MH_CANONICAL);
    PRINT_FLAG(header->flags, MH_SPLIT_SEGS);
    PRINT_FLAG(header->flags, MH_FORCE_FLAT);
    PRINT_FLAG(header->flags, MH_SUBSECTIONS_VIA_SYMBOLS);
    PRINT_FLAG(header->flags, MH_NOMULTIDEFS);
    if (header->flags == 0) {
        printf(" None");
    }
    fputc('\n', stdout);
}

local void dump_section_64(void* buffer, S(section_64*) sec64) {
    printf("  │ {C}Section 64{0}: {M+}struct {0}section_64\n");
    printf("  └─┐ Section Name: {/}\"%.16s\"{0}\n", sec64->sectname);
    printf("    │ Segment Name: {/}\"%.16s\"{0}\n", sec64->segname);
    printf("    │ Virtual Memory Address: 0x%016llx\n", sec64->addr);
    printf("    │ Virtual Memory Size: 0x%016llx\n", sec64->size);
    printf("    │ File Offset: {Y}0x%08x{0}\n", sec64->offset);
    printf("    │ Section Alignment: 2**%u\n", sec64->align);
    printf("    │ File offset of first relocation entry: {Y}0x%08x{0}\n",
           sec64->reloff);
    printf("    │ Number of first relocation entries: %u\n", sec64->nreloc);
    printf("    │ Flags: {Y}0x%08x{0}:", sec64->flags);
    PRINT_FLAG(sec64->flags, S_REGULAR);
    PRINT_FLAG(sec64->flags, S_ZEROFILL);
    PRINT_FLAG(sec64->flags, S_CSTRING_LITERALS);
    PRINT_FLAG(sec64->flags, S_4BYTE_LITERALS);
    PRINT_FLAG(sec64->flags, S_8BYTE_LITERALS);
    PRINT_FLAG(sec64->flags, S_LITERAL_POINTERS);
    PRINT_FLAG(sec64->flags, S_NON_LAZY_SYMBOL_POINTERS);
    PRINT_FLAG(sec64->flags, S_LAZY_SYMBOL_POINTERS);
    PRINT_FLAG(sec64->flags, S_SYMBOL_STUBS);
    PRINT_FLAG(sec64->flags, S_MOD_INIT_FUNC_POINTERS);
    PRINT_FLAG(sec64->flags, S_MOD_TERM_FUNC_POINTERS);
    PRINT_FLAG(sec64->flags, S_COALESCED);
    PRINT_FLAG(sec64->flags, S_GB_ZEROFILL);
    PRINT_FLAG(sec64->flags, S_ATTR_PURE_INSTRUCTIONS);
    PRINT_FLAG(sec64->flags, S_ATTR_SOME_INSTRUCTIONS);
    PRINT_FLAG(sec64->flags, S_ATTR_NO_TOC);
    PRINT_FLAG(sec64->flags, S_ATTR_EXT_RELOC);
    PRINT_FLAG(sec64->flags, S_ATTR_LOC_RELOC);
    PRINT_FLAG(sec64->flags, S_ATTR_STRIP_STATIC_SYMS);
    PRINT_FLAG(sec64->flags, S_ATTR_NO_DEAD_STRIP);
    PRINT_FLAG(sec64->flags, S_ATTR_LIVE_SUPPORT);
    if (sec64->flags == 0) {
        printf("None");
    }
    fputc('\n', stdout);
    printf("  ┌─┘ Assembly:");
    for (uint64_t i = 0; i < sec64->size; i++) {
        const unsigned char byte = ((char*)buffer + sec64->offset)[i];
        if (sec64->size > 16 && i > 4 && i < sec64->size - 4) {
            i = sec64->size - 4;
            printf(" ...");
        } else {
            printf(" 0x%02x", (uint32_t)byte);
        }
    }
    fputc('\n', stdout);
}

local void dump_segment_64(void* buffer, S(segment_command_64*) seg64) {
    printf("  │ Command Size: %u byte(s)\n", seg64->cmdsize);
    printf("  │ Segment Name: {/}\"%.16s\"{0}\n", seg64->segname);
    printf("  │ Virtual Memory Address: {Y}0x%016llx{0}\n", seg64->vmaddr);
    printf("  │ Virtual Memory Size: {Y}0x%016llx{0}\n", seg64->vmsize);
    printf("  │ File Offset: {Y}0x%016llx{0}\n", seg64->fileoff);
    printf("  │ File Size: {Y}0x%016llx{0}\n", seg64->filesize);
    printf("  │ Maximum Virtual Memory Protection: {Y}0x%08x{0}\n",
           seg64->maxprot);
    printf("  │ Initial Virtual Memory Protection: {Y}0x%08x{0}\n",
           seg64->initprot);
    printf("  │ Number of sections: %u\n", seg64->nsects);

    if (seg64->nsects > 0) {
        printf("  │ ");
    } else {
        printf("┌─┘ ");
    }
    printf("Flags: {Y}0x%08x{0}:", seg64->flags);
    PRINT_FLAG(seg64->flags, SG_HIGHVM);
    PRINT_FLAG(seg64->flags, SG_NORELOC);
    if (seg64->flags == 0) {
        printf(" None");
    }
    fputc('\n', stdout);

    char* sections = (void*)(seg64 + 1);
    for (uint32_t i = 0; i < seg64->nsects; i++) {
        S(section_64*) section = (void*)sections;
        sections += sizeof(S(section_64));//section->size;
        dump_section_64(buffer, section);
    }
    printf("┌─┘\n");
}

local void dumo_nlist64_elem(void* buffer, S(nlist_64*) elem,
                             const char* symtable) {
    printf("  │ {C}Symbol{0}: {M+}struct {0}nlist_64\n");
    printf("  └─┐ Offset in String Table: %u\n", elem->n_un.n_strx);
    printf("    │ Type: {Y}0x%02x{0}:", elem->n_type);
    PRINT_FLAG_EXT(elem->n_type, N_STAB, "(Symbolic Debugging Entry)");
    PRINT_FLAG_EXT(elem->n_type, N_PEXT, "(Private External Symbol)");
    PRINT_FLAG_EXT(elem->n_type, N_EXT, "(External Symbol)");

    const uint32_t actual_type = (elem->n_type & N_TYPE);
    fputc(' ', stdout);
    PRINT_OPTION_EXT(actual_type, N_SECT, "(Defined in Section)"); else
    PRINT_OPTION_EXT(actual_type, N_INDR, "(Indirect)"); else
    PRINT_OPTION_EXT(actual_type, N_PBUD, "(Prebound)"); else
    PRINT_OPTION_EXT(actual_type, N_ABS, "(Absolute)"); else
    PRINT_OPTION_EXT(actual_type, N_UNDF, "(Undefined)"); else {
        fputc('\n', stdout);
    }

    printf("    │ Section Location: ");
    if (elem->n_sect > 0) {
        printf("%u (from 1)\n", elem->n_sect);
    } else {
        printf("NO_SECT\n");
    }
    printf("    │ Description: {Y}0x%04x{0}\n", elem->n_desc);
    printf("    │ Address of Symbol in Assembly: {Y}0x%08x{0}\n",
           elem->n_value);
    const char* symbol = symtable + elem->n_un.n_strx;
    printf("  ┌─┘ String: offset {Y}0x%016lx{0}: {/}\"%s\"{0}\n", (void*)symbol - buffer,
           symbol);

}

local void dump_symbol_table(void* buffer, S(symtab_command*) symt) {
    printf("  │ Command Size: %u byte(s)\n", symt->cmdsize);
    printf("  │ Symbol Table Offset: %u byte(s)\n", symt->symoff);
    printf("  │ Number of Symbols: %u\n", symt->nsyms);
    printf("  │ String Table Offset: %u byte(s)\n", symt->stroff);
    if (symt->nsyms > 0) {
        printf("  │ ");
    } else {
        printf("┌─┘ ");
    }
    printf("String Table Size: %u byte(s)\n", symt->strsize);
    S(nlist_64*) syms = (void*)((char*)buffer + symt->symoff);
    char* strtbl = (char*)buffer + symt->stroff;
    for (uint32_t i = 0; i < symt->nsyms; i++) {
        dumo_nlist64_elem(buffer, syms + i, strtbl);
    }
    printf("┌─┘\n");
}

local void dump_dysym_table(void* buffer, S(dysymtab_command*) dsymt) {
    printf("  │ Command Size: %u byte(s)\n", dsymt->cmdsize);
    printf("  │ Index of first local symbol: %u\n", dsymt->ilocalsym);
    printf("  │ Number of local symbols: %u\n", dsymt->nlocalsym);
    printf("  │ Index of first external symbol: %u\n", dsymt->iextdefsym);
    printf("  │ Number of external symbols: %u\n", dsymt->nextdefsym);
    printf("  │ Index of first undefined external symbol: %u\n",
           dsymt->iundefsym);
    printf("  │ Number of undefined external symbols: %u\n", dsymt->nundefsym);
    printf("  │ File offset of table of contents: %u\n", dsymt->tocoff);
    printf("  │ Number of entries in table of contents: %u\n", dsymt->ntoc);
    printf("  │ File offset of module table: %u\n", dsymt->modtaboff);
    printf("  │ Number of entries in module table: %u\n", dsymt->nmodtab);
    printf("  │ File offset of external reference table: %u\n",
           dsymt->extrefsymoff);
    printf("  │ Number of entries in external reference table: %u\n",
           dsymt->nextrefsyms);
    printf("  │ File offset of indirect symbol table: %u\n",
           dsymt->indirectsymoff);
    printf("  │ Number of entries in indirect symbol table: %u\n",
           dsymt->nindirectsyms);
    printf("  │ File offset of external relocation table: %u\n",
           dsymt->extreloff);
    printf("  │ Number of entries in external relocation table: %u\n",
           dsymt->nextrel);
    printf("  │ File offset of local relocation table: %u\n", dsymt->locreloff);
    printf("┌─┘ Number of entries in local relocation table: %u\n",
           dsymt->nlocrel);
//    if (symt->nsyms > 0) {
//        printf("  │ ");
//    } else {
//        printf("┌─┘ ");
//    }
//    printf("┌─┘\n");
}

local void dump_build_version(void* buffer, S(build_version_command*) bver) {
    printf("  │ Command Size: %u byte(s)\n", bver->cmdsize);
    printf("  │ Platform: {Y]0x%08x{0}\n", bver->platform);
    printf("  │ Minimum OS: {Y}0x%08x{0}: %u.%u.%u\n", bver->minos, bver->minos >> 16,
           bver->minos >> 8 & 0xFF, bver->minos & 0xFF);
    printf("  │ Minimum SDK: {Y}0x%08x{0}: %u.%u.%u\n", bver->sdk, bver->sdk >> 16,
           bver->sdk >> 8 & 0xFF, bver->sdk & 0xFF);
    printf("┌─┘ Number of build tools: %u\n", bver->ntools);
}

local void dump_load_command(void* buffer, S(load_command*) load_command) {
    printf("│ {C}Load Command{0} (at offset {Y}0x%016lx{0})\n",
           (void*)load_command - buffer);
    printf("└─┐ Command Type: {Y}0x%08x{0}: ", load_command->cmd);

    if (load_command->cmd == LC_UUID) {
        printf("LC_UUID: struct uuid_command\n");
    } else if (load_command->cmd == LC_SEGMENT) {
        printf("LC_SEGMENT: struct segment_command\n");
    } else if (load_command->cmd == LC_SEGMENT_64) {
        printf("{+}LC_SEGMENT_64{0}: {M+}struct {0}segment_command_64\n");
        dump_segment_64(buffer, (S(segment_command_64*))load_command);
    } else if (load_command->cmd == LC_SYMTAB) {
        printf("{+}LC_SYMTAB{0}: {M+}struct {0}symtab_command\n");
        dump_symbol_table(buffer, (S(symtab_command*))load_command);
    } else if (load_command->cmd == LC_DYSYMTAB) {
        printf("{+}LC_DYSYMTAB{0}: {M+}struct {0}dysymtab_command\n");
        dump_dysym_table(buffer, (S(dysymtab_command*))load_command);
    } else if (load_command->cmd == LC_THREAD) {
        printf("LC_THREAD: struct thread_command\n");
    } else if (load_command->cmd == LC_UNIXTHREAD) {
        printf("LC_UNIXTHREAD: struct thread_command\n");
    } else if (load_command->cmd == LC_LOAD_DYLIB) {
        printf("LC_LOAD_DYLIB: struct dylib_command\n");
    } else if (load_command->cmd == LC_ID_DYLIB) {
        printf("LC_ID_DYLIB: struct dylib_command\n");
    } else if (load_command->cmd == LC_PREBOUND_DYLIB) {
        printf("LC_PREBOUND_DYLIB: struct prebound_dylib_command\n");
    } else if (load_command->cmd == LC_LOAD_DYLINKER) {
        printf("LC_LOAD_DYLINKER: struct dylinker_command\n");
    } else if (load_command->cmd == LC_ID_DYLINKER) {
        printf("LC_ID_DYLINKER: struct dylinker_command\n");
    } else if (load_command->cmd == LC_ROUTINES) {
        printf("LC_ROUTINES: struct routines_command\n");
    } else if (load_command->cmd == LC_ROUTINES_64) {
        printf("LC_ROUTINES_64: struct routines_command_64\n");
    } else if (load_command->cmd == LC_TWOLEVEL_HINTS) {
        printf("LC_TWOLEVEL_HINTS: struct twolevel_hints_command\n");
    } else if (load_command->cmd == LC_SUB_FRAMEWORK) {
        printf("LC_SUB_FRAMEWORK: struct sub_framework_command\n");
    } else if (load_command->cmd == LC_SUB_UMBRELLA) {
        printf("LC_SUB_UMBRELLA: struct sub_umbrella_command\n");
    } else if (load_command->cmd == LC_SUB_LIBRARY) {
        printf("LC_SUB_LIBRARY: struct sub_library_command\n");
    } else if (load_command->cmd == LC_SUB_CLIENT) {
        printf("LC_SUB_CLIENT: struct sub_client_command\n");
    } else if (load_command->cmd == LC_DYLD_INFO_ONLY) {
        printf("LC_DYLD_INFO_ONLY\n");
    } else if (load_command->cmd == LC_VERSION_MIN_MACOSX) {
        printf("LC_VERSION_MIN_MACOSX: struct version_min_command\n");
    } else if (load_command->cmd == LC_SOURCE_VERSION) {
        printf("LC_SOURCE_VERSION: struct source_version_command\n");
    } else if (load_command->cmd == LC_MAIN) {
        printf("LC_MAIN\n");
    } else if (load_command->cmd == LC_FUNCTION_STARTS) {
        printf("FUNCTION_STARTS\n");
    } else if (load_command->cmd == LC_LAZY_LOAD_DYLIB) {
        printf("LC_LAZY_LOAD_DYLIB\n");
    } else if (load_command->cmd == LC_BUILD_VERSION) {
        printf("{+}LC_BUILD_VERSION{0}: {M+}struct {0}build_version_command\n");
        dump_build_version(buffer, (S(build_version_command*))load_command);
    }

    #ifdef LC_SYMSEG
    else if (load_command->cmd == LC_SYMSEG) {
        printf("LC_SYMSEG: struct symseg_command\n");
    }
    #endif
    else {
        printf("Unknown\r├──\n");
    }
}

void mach_dump(void* buffer, const size_t length) {
    START_READ();

    S(mach_header_64*) header = READ(sizeof(*header));
    if (header->magic != MH_MAGIC_64) {
        fprintf(stderr, "machdump: {R+}error:{0} Expected 64 bit mach-o file\n");
        return;
    }
    dump_header(buffer, header);

    for (uint32_t i = 0; i < header->ncmds; i++) {
        S(load_command*) load_command = READ(sizeof(*load_command));
        CONSUME(load_command->cmdsize - sizeof(*load_command));
        dump_load_command(buffer, load_command);
    }
}
