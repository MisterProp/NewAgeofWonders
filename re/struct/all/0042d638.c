
void FUN_0042d638(int param_1,int param_2)

{
  undefined1 *puVar1;
  int iVar2;
  int *piVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_0042d704;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  iVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0xa0) + 0x114) + 0x14))();
  if (iVar2 <= param_2) {
    param_2 = (**(code **)(**(int **)(*(int *)(param_1 + 0xa0) + 0x114) + 0x14))();
    param_2 = param_2 + -1;
  }
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0xa0),param_2);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0xb0),param_2);
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF(*(undefined4 *)(param_1 + 0xa8),param_2);
  if (param_2 == -1) {
    (**(code **)(**(int **)(param_1 + 0xc0) + 0x6c))(*(int **)(param_1 + 0xc0),0);
  }
  else {
    piVar3 = *(int **)(*(int *)(param_1 + 0xa0) + 0x114);
    piVar3 = (int *)(**(code **)(*piVar3 + 0x18))(piVar3,param_2);
    uVar4 = (**(code **)(*piVar3 + 0x7c))
                      (piVar3,*(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5),&local_8)
    ;
    (**(code **)(**(int **)(param_1 + 0xc0) + 0x6c))(*(int **)(param_1 + 0xc0),uVar4);
  }
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0042d70b;
  puStack_18 = (undefined1 *)0x42d703;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

