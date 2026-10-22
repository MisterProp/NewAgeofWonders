
void FUN_00437abc(int param_1)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_18 = &LAB_00437b30;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  (**(code **)(**(int **)(param_1 + 0x7c) + 0x6c))(*(int **)(param_1 + 0x7c),1);
  (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))(*(int **)(param_1 + 0x68),1);
  (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),1);
  System_LoadResString_02614E1B(AoWE_PartyJoinRStr_8276FCC3_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x70),local_8);
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00437b37;
  puStack_18 = (undefined1 *)0x437b2f;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

