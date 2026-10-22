
void FUN_004553c0(undefined4 param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_14;
  undefined1 *puStack_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = (undefined1 *)0x4553cf;
  local_8 = param_2;
  System__LStrAddRef_51F89FF7(param_2);
  puStack_10 = &LAB_00455402;
  uStack_14 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_14;
  puStack_c = &stack0xfffffffc;
  AoWE_TAoWHSMap_SaveGame_23EDC2EF(*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref,local_8);
  puVar1 = puStack_c;
  *in_FS_OFFSET = uStack_14;
  puStack_c = &LAB_00455409;
  puStack_10 = (undefined1 *)0x455401;
  System__LStrClr_40929B27(&local_8,uStack_14,puVar1);
  return;
}

