
void FUN_00420bd4(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_2c;
  undefined1 *puStack_28;
  undefined1 *puStack_24;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  int local_8;
  
  puStack_24 = &stack0xfffffffc;
  local_8 = 0;
  puStack_28 = &LAB_00420d77;
  uStack_2c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_2c;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  if (*(int *)(*(int *)(param_1 + 0x58) + 0x120) != -1) {
    piVar1 = *(int **)(*(int *)(param_1 + 0x58) + 0x114);
    (**(code **)(*piVar1 + 0xc))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x58) + 0x120),&local_8);
  }
  iVar3 = *(int *)(param_1 + 0x148);
  if ((iVar3 == 1) || (iVar3 == 0)) {
    iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0xd8) + 0x114) + 0x14))();
    if (-1 < iVar3 + -1) {
      iVar5 = 0;
      local_c = iVar3;
      do {
        piVar1 = *(int **)(*(int *)(param_1 + 0xe8) + 0x114);
        uVar4 = (**(code **)(*piVar1 + 0x18))(piVar1,iVar5);
        piVar1 = *(int **)(*(int *)(param_1 + 0xd8) + 0x114);
        (**(code **)(*piVar1 + 0x24))(piVar1,iVar5,uVar4);
        iVar5 = iVar5 + 1;
        local_c = local_c + -1;
      } while (local_c != 0);
    }
    local_14 = *(undefined4 *)(*(int *)(param_1 + 0xe8) + 0x114);
    local_10 = *(undefined4 *)(*(int *)(param_1 + 0x58) + 0x114);
    Engine_SortStringsEx_0A456501
              (*(undefined4 *)(*(int *)(param_1 + 0xd8) + 0x114),FUN_00420b80,&local_14,1);
  }
  else if (iVar3 == 2) {
    iVar3 = (**(code **)(**(int **)(*(int *)(param_1 + 0x58) + 0x114) + 0x14))();
    if (-1 < iVar3 + -1) {
      iVar5 = 0;
      local_c = iVar3;
      do {
        piVar1 = *(int **)(*(int *)(param_1 + 0xe8) + 0x114);
        uVar4 = (**(code **)(*piVar1 + 0x18))(piVar1,iVar5);
        piVar1 = *(int **)(*(int *)(param_1 + 0x58) + 0x114);
        (**(code **)(*piVar1 + 0x24))(piVar1,iVar5,uVar4);
        iVar5 = iVar5 + 1;
        local_c = local_c + -1;
      } while (local_c != 0);
    }
    local_14 = *(undefined4 *)(*(int *)(param_1 + 0xe8) + 0x114);
    local_10 = *(undefined4 *)(*(int *)(param_1 + 0xd8) + 0x114);
    Engine_SortStringsEx_0A456501
              (*(undefined4 *)(*(int *)(param_1 + 0x58) + 0x114),FUN_00420b80,&local_14,1);
  }
  if (local_8 != 0) {
    iVar3 = *(int *)(param_1 + 0x58);
    piVar1 = *(int **)(iVar3 + 0x114);
    uVar4 = (**(code **)(*piVar1 + 0x50))(piVar1,local_8);
    AoWListBox_TAOWListBox_SetIndex_23EDC2EF(iVar3,uVar4);
  }
  FUN_004201ec(param_1,*(undefined4 *)(param_1 + 0x58));
  puVar2 = puStack_24;
  *in_FS_OFFSET = uStack_2c;
  puStack_24 = &LAB_00420d7e;
  puStack_28 = (undefined1 *)0x420d76;
  System__LStrClr_40929B27(&local_8,uStack_2c,puVar2);
  return;
}

