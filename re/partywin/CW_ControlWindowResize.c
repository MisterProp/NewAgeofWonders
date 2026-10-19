
void FUN_00406040(undefined4 param_1,int param_2,char param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  if ((*(int *)(*(int *)PTR_DAT_0045a420 + 0xa4) != 0) &&
     (-1 < *(int *)(*(int *)PTR_DAT_0045a420 + 0x84 + *(int *)(param_2 + 0xc) * 4))) {
    iVar1 = AoWE_TUnitList_GetUnit_23EDC2EF
                      (*(int *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                       *(undefined4 *)
                        (*(int *)PTR_DAT_0045a420 + 0x84 + *(int *)(param_2 + 0xc) * 4));
    if (iVar1 != 0) {
      uVar2 = AoWE_TUnitList_GetUnit_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xa4),
                         *(undefined4 *)
                          (*(int *)PTR_DAT_0045a420 + 0x84 + *(int *)(param_2 + 0xc) * 4));
      *(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0) = uVar2;
      if (param_3 == '\0') {
        FUN_00407db8(*(undefined4 *)PTR_DAT_0045a53c);
      }
    }
  }
  return;
}

