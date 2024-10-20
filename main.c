#include "depends.h"
#include <mach/mach.h>
#include <pongo.h>

extern struct mach_header_64 *xnu_header();

#define TARGET_OFFSET 0x00F25C28 // Offset of the target instruction
#define TARGET_INST 0x9a9f132d   // CSEL X13, X25, XZR, NE
#define REPLACE_INST 0xAA1F03ED  // MOV X13, XZR

void disable_aslr() {
  struct mach_header_64 *hdr = xnu_header();
  if (hdr == NULL) {
    panic("Failed to get XNU mach header");
  }
  print_hex64("Header: ", (uint64_t)hdr);

  uint64_t addr = (uint64_t)hdr + TARGET_OFFSET;
  uint32_t target = *(uint32_t *)addr;

  print_hex32("target: ", target);

  if (target != TARGET_INST && target != REPLACE_INST) {
    panic("Invalid target instruction");
  }

  if (target == REPLACE_INST) {
    puts("already patched");
    return;
  }

  puts("Patching target instruction");
  *(uint32_t *)addr = REPLACE_INST;
}

void module_entry() {
  command_register("disable_aslr", "Disable aslr", disable_aslr);
}

char *module_name = "disable_aslr";

struct pongo_exports exported_symbols[] = {{.name = 0, .value = 0}};
