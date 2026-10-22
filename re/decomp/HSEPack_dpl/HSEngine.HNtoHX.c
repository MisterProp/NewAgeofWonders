
void HSEngine_HNtoHX_56E4633A(int param_1,uint *param_2,int *param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
                    /* 0xe08c  69  HSEngine.HNtoHX@56E4633A */
  if (param_1 < 0x25) {
    *param_2 = *(uint *)(&HSEngine_HNtoHXTable_64B540B1 + param_1 * 8);
    *param_3 = *(int *)(&DAT_5562e0f8 + param_1 * 8);
  }
  else {
    iVar1 = System__TRUNC_51F89FF7();
    uVar2 = (iVar1 + 3) / 6;
    iVar3 = param_1 + (uVar2 - 1) * uVar2 * -3 + -1;
    iVar1 = iVar3 % (int)uVar2;
    switch(iVar3 / (int)uVar2) {
    case 0:
      HSEngine_HVtoHX_8E9B8701(uVar2,iVar1 - uVar2,param_2,param_3);
      break;
    case 1:
      *param_2 = uVar2;
      iVar3 = (int)(uVar2 + 1) >> 1;
      if (iVar3 < 0) {
        iVar3 = iVar3 + (uint)((uVar2 + 1 & 1) != 0);
      }
      *param_3 = iVar1 - iVar3;
      break;
    case 2:
      HSEngine_HVtoHX_8E9B8701(-iVar1,uVar2,param_2,param_3);
      break;
    case 3:
      HSEngine_HVtoHX_8E9B8701(-uVar2,uVar2 - iVar1,param_2,param_3);
      break;
    case 4:
      *param_2 = -uVar2;
      iVar3 = (int)uVar2 >> 1;
      if (iVar3 < 0) {
        iVar3 = iVar3 + (uint)((uVar2 & 1) != 0);
      }
      *param_3 = iVar3 - iVar1;
      break;
    case 5:
      HSEngine_HVtoHX_8E9B8701(iVar1,-uVar2,param_2,param_3);
    }
  }
  return;
}

