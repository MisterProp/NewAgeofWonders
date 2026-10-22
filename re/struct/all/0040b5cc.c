
undefined4 FUN_0040b5cc(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  
  iVar1 = *(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4);
  if (iVar1 == *(int *)(*(int *)(*(int *)PTR_DAT_0045a168 + 0x1dc) + 0x1a8)) {
    iVar2 = *(int *)(iVar1 + 0x60);
    iVar1 = *(int *)(iVar1 + 100);
    iVar3 = GFXE_TDisplayModeList_GetCount_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x8c),PTR_DAT_0045a168);
    do {
      iVar3 = iVar3 + -1;
      if (iVar3 < 0) break;
      iVar4 = GFXE_TDisplayModeList_GetItems_23EDC2EF(*(undefined4 *)(param_1 + 0x8c),iVar3);
    } while ((iVar2 != *(int *)(iVar4 + 8)) ||
            (iVar4 = GFXE_TDisplayModeList_GetItems_23EDC2EF(*(undefined4 *)(param_1 + 0x8c),iVar3),
            iVar1 != *(int *)(iVar4 + 0xc)));
    uVar5 = Classes_TStrings_IndexOfObject_23EDC2EF
                      (*(undefined4 *)(*(int *)(param_1 + 0x70) + 0x114),iVar3);
  }
  else {
    uVar5 = 0;
  }
  return uVar5;
}

