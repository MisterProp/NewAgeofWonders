
void FUN_00427818(undefined4 param_1,int param_2)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_18 = &LAB_0042789b;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  puVar1 = &stack0xfffffffc;
  if (*(int *)(param_2 + 0x200) == 0) {
    System_LoadResString_02614E1B(PTR_PTR_0045a178,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    System_LoadResString_02614E1B(PTR_PTR_0045a430,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    FUN_004505bc(*(undefined4 *)PTR_DAT_0045a5fc,local_c,6);
    puVar1 = puStack_14;
  }
  puStack_14 = puVar1;
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_004278a2;
  puStack_18 = (undefined1 *)0x42789a;
  System__LStrArrayClr_51F89FF7(&local_10,3,puVar1);
  return;
}

