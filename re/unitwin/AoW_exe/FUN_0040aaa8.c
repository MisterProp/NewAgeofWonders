// /AoW.exe : FUN_0040aaa8

void FUN_0040aaa8(void)

{
  undefined1 *puVar1;
  undefined4 uVar2;
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
  puStack_18 = &LAB_0040ab20;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  System_LoadResString_02614E1B(PTR_PTR_0045a250,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  uVar2 = local_8;
  System_LoadResString_02614E1B(PTR_PTR_0045a430,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  FUN_004505bc(*(undefined4 *)PTR_DAT_0045a5fc,local_c,uVar2);
  puVar1 = puStack_18;
  *in_FS_OFFSET = 7;
  puStack_18 = &LAB_0040ab27;
  uStack_1c = 0x40ab1f;
  System__LStrArrayClr_51F89FF7(&local_10,3,puVar1);
  return;
}

