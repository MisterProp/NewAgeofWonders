
void FUN_0040701c(int param_1,undefined4 param_2,int *param_3)

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
  puStack_18 = &LAB_0040706e;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x44));
  (**(code **)(*param_3 + 0x1c))(param_3,&local_8);
  piVar1 = *(int **)(*(int *)(param_1 + 0x48) + 0x118);
  (**(code **)(*piVar1 + 0x2c))(piVar1,local_8);
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00407075;
  puStack_18 = (undefined1 *)0x40706d;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar2);
  return;
}

