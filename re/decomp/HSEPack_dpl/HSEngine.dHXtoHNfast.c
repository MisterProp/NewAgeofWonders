
undefined4 HSEngine_dHXtoHNfast_F769B863(uint param_1,int param_2,uint param_3,int param_4)

{
  undefined4 uVar1;
  
                    /* 0xe338  64  HSEngine.dHXtoHNfast@F769B863 */
  if (param_3 == param_1) {
    if (param_4 < param_2) {
      uVar1 = 1;
    }
    else if (param_2 < param_4) {
      uVar1 = 4;
    }
    else {
      uVar1 = 0;
    }
  }
  else if ((param_1 & 1) == 0) {
    if ((int)param_3 < (int)param_1) {
      if (param_4 < param_2) {
        uVar1 = 6;
      }
      else {
        uVar1 = 5;
      }
    }
    else if (param_4 < param_2) {
      uVar1 = 2;
    }
    else {
      uVar1 = 3;
    }
  }
  else if ((int)param_3 < (int)param_1) {
    if (param_2 < param_4) {
      uVar1 = 5;
    }
    else {
      uVar1 = 6;
    }
  }
  else if (param_2 < param_4) {
    uVar1 = 3;
  }
  else {
    uVar1 = 2;
  }
  return uVar1;
}

