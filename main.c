#include "depends.h"
#include "pongo.h"
#include <mach-o/loader.h>

extern struct mach_header_64 *xnu_header();
static struct mach_header_64 *header = NULL;

#define REPLACE_INST 0xd2800408 // MOV x8, #0x20

uint32_t *find_next_insn(uint32_t *from, uint32_t num, uint32_t insn,
                         uint32_t mask) {
  while (num) {
    if ((*from & mask) == (insn & mask)) {
      return from;
    }
    from++;
    num--;
  }
  return NULL;
}


bool disable_aslr_callback(struct xnu_pf_patch *patch,
                                 uint32_t *opcode_stream) {

  puts("we entered into disable_aslr_callback function");
  uint32_t *is_patched = find_next_insn(opcode_stream, 0x18, REPLACE_INST, 0xffffffff); // mov w8, 0x20 // 0xd2800408
  if (is_patched != NULL) {
    print_hex64("Target instruction already patched at: ",
                (uint64_t)is_patched - (uint64_t)header);
    return true;
  }
  uint32_t *found = find_next_insn(opcode_stream, 0x18, 0x394122c8, 0xffff0000); // ldrb    w8, [x22, #0x48] // C8224139
  if (found == NULL) {
    return false;
  }

  if ((*(found+0x1) & 0xff000000) == 0xb9000000) { // check if str on ios 14
    if ((*(found+0x2) & 0xffff000f) != 0x37280008) {
      puts("wrong instruction ignoring ...");
      return false;
    }
  } else if ((*(found+0x1) & 0xffff000f) != 0x37280008) { // check if tbnz
    puts("wrong instruction ignoring ...");
    return false;
  }
  print_hex64("found at offset: ", (uint64_t)found - (uint64_t)header);
  print_hex32("target: ", (uint32_t)*found);

  puts("Patching target instruction");
  *found = REPLACE_INST;
  puts("Patched target instruction");
  return true;
}

void disable_aslr() {
  header = xnu_header();
  if (header == NULL) {
    panic("Failed to get XNU mach header");
  }
  print_hex64("Header: ", (uint64_t)header);

  // https://github.com/checkra1n/PongoOS/blob/4c9b7541629234147fcc778f0ce4162482aaccef/checkra1n/kpf/main.c#L1628
  xnu_pf_patchset_t *text_exec_patchset = xnu_pf_patchset_create(XNU_PF_ACCESS_32BIT);
  xnu_pf_range_t *text_exec_range = xnu_pf_section(header, "__TEXT_EXEC", "__text");

  struct mach_header_64 *first_kext = xnu_pf_get_first_kext(header);
  if (first_kext) {
    xnu_pf_range_t *first_kext_text_exec_range =
        xnu_pf_section(first_kext, "__TEXT_EXEC", "__text");

    if (first_kext_text_exec_range) {
      uint64_t text_exec_end_real;
      uint64_t text_exec_end = text_exec_end_real =
          ((uint64_t)(text_exec_range->va)) + text_exec_range->size;
      uint64_t first_kext_p = ((uint64_t)(first_kext_text_exec_range->va));

      if (text_exec_end > first_kext_p &&
          first_kext_text_exec_range->va > text_exec_range->va) {
        text_exec_end = first_kext_p;
      }

      text_exec_range->size -= text_exec_end_real - text_exec_end;
    }
  }

  uint64_t matches[] = {
    0xb9400008, // ldr     w8, [x0, #0x0] // ldr     w8, [x27, #0x10c]
    0x321a0108, // orr     w8, w8, #0x40
    0xb9000008, // str     w8, [x0, #0x0] // str     w8, [x27, #0x10c]
                // ldrb    w8, [x22, #0x48] // this the instruction to path
  };
  uint64_t masks[] = {
    0xfff0000f,
    0xffffffff,
    0xff00000f,
  };

  xnu_pf_maskmatch(text_exec_patchset, "disable_aslr", matches, masks,
                   sizeof(matches) / sizeof(uint64_t), false,
                   (void *)disable_aslr_callback);

  xnu_pf_emit(text_exec_patchset);
  xnu_pf_apply(text_exec_range, text_exec_patchset);
  xnu_pf_patchset_destroy(text_exec_patchset);
}

void module_entry() { disable_aslr(); }

char *module_name = "disable_aslr";

struct pongo_exports exported_symbols[] = {{.name = 0, .value = 0}};
