
void FUN_0040f440(int param_1)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  undefined4 local_8;
  
  puStack_10 = &stack0xfffffffc;
  local_8 = 0;
  puStack_14 = &LAB_0040f4b0;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  *(undefined4 *)(param_1 + 0xe4) = 0;
  (**(code **)(**(int **)(param_1 + 0xe0) + 0x40))();
  (**(code **)(**(int **)(param_1 + 0x7c) + 0x6c))(*(int **)(param_1 + 0x7c),1);
  AoWVScroll_TAOWScrollBar_SetFPos_23EDC2EF(*(undefined4 *)(param_1 + 0x7c),0);
  AoWReg_TAoWRegistry_GetLocalEmailAddress_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),&local_8);
  AoWEdit_TAOWEdit_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x68),local_8);
  puVar1 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_0040f4b7;
  puStack_14 = (undefined1 *)0x40f4af;
  System__LStrClr_40929B27(&local_8,uStack_18,puVar1);
  return;
}

