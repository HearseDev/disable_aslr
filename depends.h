#pragma once
#include <pongo.h>

// Function to convert uint32_t to hex string
void uint32ToHexStr(uint32_t num, char str[]) {
  const char hexDigits[] = "0123456789ABCDEF";
  int i = 0;

  // Process each byte in the 32-bit number (8 hex digits)
  for (int shift = 28; shift >= 0; shift -= 4) {
    int nibble = (num >> shift) & 0xF; // Extract 4 bits (1 nibble)
    str[i++] = hexDigits[nibble];      // Convert nibble to hex char
  }

  // Null-terminate the string
  str[i] = '\0';
}

// Function to convert uint64_t to hex string
void uint64ToHexStr(uint64_t num, char str[]) {
  const char hexDigits[] = "0123456789ABCDEF";
  int i = 0;

  // Process each byte in the 64-bit number (16 hex digits)
  for (int shift = 60; shift >= 0; shift -= 4) {
    int nibble = (num >> shift) & 0xF; // Extract 4 bits (1 nibble)
    str[i++] = hexDigits[nibble];      // Convert nibble to hex char
  }

  // Null-terminate the string
  str[i] = '\0';
}

void *pmemmove(void *dst0, const void *src0, size_t length) {
  typedef long word; /* "word" used for optimal copy speed */

#define wsize sizeof(word)
#define wmask (wsize - 1)

  char *dst = (char *)dst0;
  const char *src = (char *)src0;
  size_t t;

  if (length == 0 || dst == src) /* nothing to do */
    goto done;

    /*
     * Macros: loop-t-times; and loop-t-times, t>0
     */
#define TLOOP(s)                                                               \
  if (t)                                                                       \
  TLOOP1(s)
#define TLOOP1(s)                                                              \
  do {                                                                         \
    s;                                                                         \
  } while (--t)

  if ((unsigned long)dst < (unsigned long)src) {
    /*
     * Copy forward.
     */
    t = (uintptr_t)src; /* only need low bits */
    if ((t | (uintptr_t)dst) & wmask) {
      /*
       * Try to align operands.  This cannot be done
       * unless the low bits match.
       */
      if ((t ^ (uintptr_t)dst) & wmask || length < wsize)
        t = length;
      else
        t = wsize - (t & wmask);
      length -= t;
      TLOOP1(*dst++ = *src++);
    }
    /*
     * Copy whole words, then mop up any trailing bytes.
     */
    t = length / wsize;
    TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
    t = length & wmask;
    TLOOP(*dst++ = *src++);
  } else {
    /*
     * Copy backwards.  Otherwise essentially the same.
     * Alignment works as before, except that it takes
     * (t&wmask) bytes to align, not wsize-(t&wmask).
     */
    src += length;
    dst += length;
    t = (uintptr_t)src;
    if ((t | (uintptr_t)dst) & wmask) {
      if ((t ^ (uintptr_t)dst) & wmask || length <= wsize)
        t = length;
      else
        t &= wmask;
      length -= t;
      TLOOP1(*--dst = *--src);
    }
    t = length / wsize;
    TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
    t = length & wmask;
    TLOOP(*--dst = *--src);
  }
done:
  return (dst0);
}
size_t pstrlen(const char *str) {
  const char *s = str;
  while (*s)
    ++s;
  return s - str;
}

void print_hex32(const char *title, uint32_t num) {
  char str[64];
  size_t len = pstrlen(title);
  pmemmove(str, title, len);
  uint32ToHexStr(num, str + len);
  puts(str);
}
void print_hex64(const char *title, uint64_t num) {
  char str[64];
  size_t len = pstrlen(title);
  pmemmove(str, title, len);
  uint64ToHexStr(num, str + len);
  puts(str);
}