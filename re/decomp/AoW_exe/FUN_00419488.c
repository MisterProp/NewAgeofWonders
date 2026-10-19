// /AoW.exe : FUN_00419488

int FUN_00419488(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  
  if ((*(int *)(param_1 + 0x90) + -0x7d < *(int *)(param_1 + 0x74)) &&
     (*(int *)(param_1 + 0x74) < *(int *)(param_1 + 0x90))) {
    iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x44),10);
    uVar4 = *(int *)(iVar2 + 0x20) + *(int *)(param_1 + 0x84) + *(int *)(param_1 + 0x8c);
    iVar5 = (int)uVar4 >> 1;
    if (iVar5 < 0) {
      iVar5 = iVar5 + (uint)((uVar4 & 1) != 0);
    }
    iVar2 = 5;
    bVar1 = false;
    while ((iVar2 != 0 && (!bVar1))) {
      iVar2 = iVar2 + -1;
      if (iVar2 * 0x78 + iVar5 + -0x140 < *(int *)(param_1 + 0x70)) {
        iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x44),10);
        if (*(int *)(iVar3 + 0x20) + iVar5 + -0x140 + iVar2 * 0x78 <= *(int *)(param_1 + 0x70))
        goto LAB_00419507;
        bVar1 = true;
      }
      else {
LAB_00419507:
        bVar1 = false;
      }
    }
    if (!bVar1) {
      iVar2 = -1;
    }
  }
  else {
    iVar2 = -1;
  }
  return iVar2;
}

