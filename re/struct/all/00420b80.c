
undefined4 FUN_00420b80(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  
  if ((param_3 == 0) && (param_4 == 0)) {
    uVar1 = SysUtils_CompareText_8779A639();
  }
  else if (param_3 == 0) {
    uVar1 = 0xffffffff;
  }
  else if (param_4 == 0) {
    uVar1 = 1;
  }
  else if (*(double *)(param_4 + 4) <= *(double *)(param_3 + 4)) {
    if (*(double *)(param_3 + 4) <= *(double *)(param_4 + 4)) {
      uVar1 = 0;
    }
    else {
      uVar1 = 0xffffffff;
    }
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

