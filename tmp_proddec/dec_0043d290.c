
undefined4 FUN_0043d290(int param_1,uint param_2)

{
  undefined4 uVar1;
  
  switch(param_2 & 0x7f) {
  default:
    uVar1 = 0x28;
    break;
  case 1:
    return 0x28;
  case 2:
    return 0x2b;
  case 3:
    if (*(char *)(*(int *)(param_1 + 0x2b0) + 0x4c) != '\0') {
      return 0x2a;
    }
    uVar1 = 0x29;
    break;
  case 4:
    return 0x2c;
  case 5:
    return 0x2d;
  case 6:
    return 0x2e;
  case 7:
    return 5;
  case 8:
    return 4;
  }
  return uVar1;
}

