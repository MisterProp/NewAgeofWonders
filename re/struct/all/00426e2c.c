
void FUN_00426e2c(int param_1)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  undefined4 local_8;
  
  puStack_10 = &stack0xfffffffc;
  local_8 = 0;
  puStack_14 = &LAB_00426e93;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  if (*(int *)(param_1 + 0x27c) != 0) {
    *(undefined4 *)(*(int *)(param_1 + 0x27c) + 0x1c) =
         *(undefined4 *)(*(int *)(param_1 + 0x224) + 0x148);
  }
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(*(int *)(param_1 + 0x224) + 0x148),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x214),local_8);
  puVar1 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_00426e9a;
  puStack_14 = (undefined1 *)0x426e92;
  System__LStrClr_40929B27(&local_8,uStack_18,puVar1);
  return;
}

