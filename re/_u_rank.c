
undefined4 AoWE_TUnit_GetRank_23EDC2EF(int param_1)

{
  undefined3 uVar2;
  undefined4 uVar1;
  int iVar3;
  
                    /* 0x8294c  945  AoWE.TUnit.GetRank@23EDC2EF */
  iVar3 = (int)*(char *)(*(int *)(param_1 + 0x40) + 0x2f);
  if ((int)(uint)*(byte *)(param_1 + 0x3c) < iVar3 * 2) {
    uVar1 = 0;
  }
  else {
    iVar3 = iVar3 * 6;
    uVar2 = (undefined3)((uint)iVar3 >> 8);
    if ((int)(uint)*(byte *)(param_1 + 0x3c) < iVar3) {
      return CONCAT31(uVar2,1);
    }
    uVar1 = CONCAT31(uVar2,2);
  }
  return uVar1;
}

