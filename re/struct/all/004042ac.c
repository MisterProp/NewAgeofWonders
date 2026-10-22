
void FUN_004042ac(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  int *piVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00404357;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  puVar1 = &stack0xfffffffc;
  if (*(int *)(*(int *)PTR_DAT_0045a420 + 0xc0) != 0) {
    iVar2 = *(int *)(*(int *)(param_1 + 0x7c) + 0x120);
    if ((iVar2 < 0) || (*(int *)(*(int *)PTR_DAT_0045a420 + 0x7c) <= iVar2)) {
      AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(int *)(param_1 + 0x7c),0xffffffff);
      puVar1 = puStack_14;
    }
    else {
      puStack_14 = &stack0xfffffffc;
      iVar2 = Classes_TList_Get_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x78),iVar2);
      piVar3 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x80),
                                 *(undefined4 *)(iVar2 + 4));
      (**(code **)(*piVar3 + 0xb4))
                (piVar3,*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0xc0),&local_8);
      (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),0);
      puVar1 = puStack_14;
    }
  }
  puStack_14 = puVar1;
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0040435e;
  puStack_18 = (undefined1 *)0x404356;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

