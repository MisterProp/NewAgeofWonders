
undefined4 FUN_004541b0(undefined4 param_1,byte param_2)

{
  undefined4 uVar1;
  
  uVar1 = 0;
  if ((param_2 & 8) == 8) {
    uVar1 = 2;
  }
  if ((param_2 & 0x20) == 0x20) {
    uVar1 = 4;
  }
  if ((param_2 & 4) == 4) {
    uVar1 = 6;
  }
  if ((param_2 & 5) == 5) {
    uVar1 = 8;
  }
  if ((param_2 & 0xc) == 0xc) {
    uVar1 = 10;
  }
  if ((param_2 & 0x24) == 0x24) {
    uVar1 = 0xc;
  }
  if ((param_2 & 0x28) == 0x28) {
    uVar1 = 0xe;
  }
  if ((param_2 & 0x2c) == 0x2c) {
    uVar1 = 0x10;
  }
  if ((param_2 & 2) == 2) {
    uVar1 = 0x12;
  }
  return uVar1;
}

