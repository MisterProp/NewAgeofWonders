
void FUN_0040e7e4(int param_1)

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
  puStack_18 = &LAB_0040e890;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  (**(code **)(**(int **)(param_1 + 0x4c) + 0x6c))(*(int **)(param_1 + 0x4c),0);
  (**(code **)(**(int **)(param_1 + 0x88) + 0x6c))(*(int **)(param_1 + 0x88),0);
  (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))(*(int **)(param_1 + 0x70),0);
  (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),0);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x80) + 0x118) + 0x40))();
  (**(code **)(**(int **)(param_1 + 0x98) + 0x6c))(*(int **)(param_1 + 0x98),1);
  (**(code **)(**(int **)(param_1 + 0x9c) + 0x6c))(*(int **)(param_1 + 0x9c),0);
  System_LoadResString_02614E1B(AoWE_MessageRStr_8276FCC3_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x48),local_8);
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0040e897;
  puStack_18 = (undefined1 *)0x40e88f;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

