
void FUN_0044cc2c(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_0044cd25;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_0044cbac(param_1);
  *(undefined4 *)(param_1 + 0xa4) = param_4;
  *(undefined4 *)(param_1 + 0xa8) = param_5;
  *(int **)(param_1 + 0xac) = param_2;
  (**(code **)(*param_2 + 0x28))();
  iVar3 = *(int *)(param_1 + 0xac);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 0x48),*(undefined4 *)(iVar3 + 0x20));
  (**(code **)(**(int **)(iVar3 + 0x1c) + 0x1c))(*(int **)(iVar3 + 0x1c),&local_8);
  piVar1 = *(int **)(*(int *)(param_1 + 0x74) + 0x118);
  (**(code **)(*piVar1 + 0x2c))(piVar1,local_8);
  (**(code **)(**(int **)(param_1 + 0x98) + 0x6c))
            (*(int **)(param_1 + 0x98),5 < *(int *)(*(int *)(iVar3 + 0x18) + 8));
  (**(code **)(**(int **)(param_1 + 0x9c) + 0x6c))
            (*(int **)(param_1 + 0x9c),5 < *(int *)(*(int *)(iVar3 + 0x18) + 8));
  iVar3 = FUN_0044cbd0(param_1);
  if (iVar3 == 0) {
    (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))(*(int **)(param_1 + 0x70),1);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x4c) + 0x6c))(*(int **)(param_1 + 0x4c),1);
  }
  AoWE_TTutorialControl_ShowMessage_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x36,1);
  piVar1 = *(int **)(param_1 + 0x44);
  *(undefined1 *)(piVar1 + 0x29) = 0;
  (**(code **)(*piVar1 + 0x6c))(piVar1,1);
  puVar2 = puStack_18;
  *in_FS_OFFSET = 1;
  puStack_18 = &LAB_0044cd2c;
  uStack_1c = 0x44cd24;
  System__LStrClr_40929B27(&local_8,1,puVar2);
  return;
}

