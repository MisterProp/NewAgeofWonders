
void FUN_004428b8(int param_1,int *param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_0044295e;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  (**(code **)(**(int **)(param_1 + 0x158) + 0x6c))(*(int **)(param_1 + 0x158),1);
  uVar3 = (**(code **)(*param_2 + 0x144))();
  piVar1 = *(int **)(*(int *)(param_1 + 0x16c) + 0x118);
  (**(code **)(*piVar1 + 8))(piVar1,uVar3);
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x160),0x21);
  uVar3 = (**(code **)(*param_2 + 0x1f8))();
  SysUtils_IntToStr_0F6FDFF6(uVar3,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x164),local_8);
  (**(code **)(**(int **)(param_1 + 0x10c) + 0x6c))(*(int **)(param_1 + 0x10c),1);
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00442965;
  puStack_18 = (undefined1 *)0x44295d;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

