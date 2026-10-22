
void FUN_00447858(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_004478ef;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  *(undefined4 *)(param_1 + 0xa0) = param_4;
  *(undefined4 *)(param_1 + 0xa4) = param_5;
  *(int **)(param_1 + 0xa8) = param_2;
  (**(code **)(*param_2 + 0x28))();
  (**(code **)(*(int *)param_2[8] + 0x1c))((int *)param_2[8],&local_8);
  piVar1 = *(int **)(*(int *)(param_1 + 0x88) + 0x118);
  (**(code **)(*piVar1 + 0x2c))(piVar1,local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x74),param_2[9]);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x94),param_2[0xb]);
  (**(code **)(**(int **)(param_1 + 0x7c) + 0x6c))(*(int **)(param_1 + 0x7c),1);
  puVar2 = puStack_14;
  *(undefined1 *)(param_1 + 0x9c) = 0;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_004478f6;
  puStack_18 = (undefined1 *)0x4478ee;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

