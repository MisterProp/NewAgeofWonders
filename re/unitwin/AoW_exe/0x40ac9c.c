// /AoW.exe : 0x40ac9c

void FUN_0040ac9c(undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  if (*(int *)(*(int *)PTR_DAT_0045a420 + 0xa4) != 0) {
    iVar1 = FUN_00454100(*(undefined4 *)PTR_DAT_0045a420,
                         *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0));
    if (-1 < iVar1) {
      iVar1 = iVar1 + 1;
      if (7 < iVar1) {
        iVar1 = 0;
      }
      if (*(int *)(*(int *)PTR_DAT_0045a420 + 0x84 + iVar1 * 4) == -1) {
        iVar1 = 0;
      }
      uVar2 = AoWE_TUnitList_GetUnit_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                         *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x84 + iVar1 * 4));
      *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0) = uVar2;
      FUN_00407db8(param_1);
    }
  }
  return;
}

