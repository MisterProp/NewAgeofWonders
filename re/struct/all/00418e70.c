
void FUN_00418e70(int param_1)

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
  puStack_18 = &LAB_00418eec;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  puVar1 = &stack0xfffffffc;
  if ((*(char *)(*(int *)(param_1 + 0x6c) + 0x75) == '\0') &&
     (puVar1 = &stack0xfffffffc, *(char *)(*(int *)(param_1 + 0x78) + 0x75) == '\0')) {
    (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),1);
    (**(code **)(**(int **)(param_1 + 0x5c) + 0x6c))(*(int **)(param_1 + 0x5c),0);
    System_LoadResString_02614E1B(PTR_PTR_0045a12c,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x48),local_8);
    puVar1 = puStack_14;
  }
  puStack_14 = puVar1;
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00418ef3;
  puStack_18 = (undefined1 *)0x418eeb;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

