
void FUN_004505bc(int param_1,undefined4 param_2,undefined4 param_3,undefined1 param_4)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_14 = (undefined1 *)0x4505d3;
  local_c = param_3;
  local_8 = param_2;
  System__LStrAddRef_51F89FF7(param_2);
  puStack_14 = (undefined1 *)0x4505db;
  System__LStrAddRef_51F89FF7(local_c);
  puStack_18 = &LAB_00450641;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  *(undefined1 *)(param_1 + 0xa4) = param_4;
  puStack_14 = &stack0xfffffffc;
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x48),local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x4c),local_c);
  (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),1);
  (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))(*(int **)(param_1 + 0x70),0);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00450648;
  puStack_18 = (undefined1 *)0x450640;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

