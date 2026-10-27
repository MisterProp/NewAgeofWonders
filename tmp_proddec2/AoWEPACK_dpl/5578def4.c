// /AoWEPACK.dpl : 5578def4

uint AoWE_TArmy_MaxSize_23EDC2EF(int *param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
                    /* 0x8def4  386  AoWE.TArmy.MaxSize@23EDC2EF */
  uVar4 = 8;
  iVar1 = (**(code **)(*param_1 + 0x54))();
  if (-1 < iVar1 + -1) {
    iVar3 = 0;
    do {
      uVar2 = (**(code **)(**(int **)(*(int *)(param_1[2] + 4) + iVar3 * 4) + 0x104))();
      uVar2 = uVar2 & 0xff;
      if ((uVar2 != 0) && (uVar2 + 1 < uVar4)) {
        uVar4 = uVar2 + 1;
      }
      iVar3 = iVar3 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  return uVar4;
}

