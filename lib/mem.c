#include "./mem.h"
#include <sizedtypes.h>

static u8 a[2] = { 10, 11 };

u8* getMemAddr(u32 index) {
  return &a[index];
}

u8 getMem(u32 index) {
  return a[index];
}

u8 setMem(u32 index, u8 val) {
  a[index] = val;
  return getMem(index);
}
