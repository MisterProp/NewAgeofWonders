
void FUN_0041bbc4(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_18 = &LAB_0041bc42;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  iVar2 = FUN_0041bbb8(param_1);
  *(undefined4 *)(iVar2 + 0x38) = 0;
  iVar2 = FUN_0041bbb8(param_1);
  *(undefined4 *)(iVar2 + 0x3c) = 0;
  (**(code **)(**(int **)(param_1 + 0x8c) + 0x6c))(*(int **)(param_1 + 0x8c),0);
  System_LoadResString_02614E1B(PTR_PTR_0045a5b8,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x100),local_8);
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0041bc49;
  puStack_18 = (undefined1 *)0x41bc41;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

