
void FUN_0042b7c0(void)

{
  undefined1 *puVar1;
  LPCSTR lpPathName;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_14;
  undefined1 *puStack_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &stack0xfffffffc;
  local_8 = 0;
  puStack_10 = &LAB_0042b83a;
  uStack_14 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_14;
  *(undefined4 *)(*(int *)PTR_DAT_0045a4c8 + 0x148) = 0;
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a4c8 + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a4c8 + 0x44),1);
  System__LStrCat3_51F89FF7
            (&local_8,*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c),"EmailIn\\");
  lpPathName = (LPCSTR)System__LStrToPChar_51F89FF7(local_8);
  SetCurrentDirectoryA(lpPathName);
  FUN_0041f430(*(undefined4 *)PTR_DAT_0045a4c8);
  puVar1 = puStack_c;
  *in_FS_OFFSET = uStack_14;
  puStack_c = &LAB_0042b841;
  puStack_10 = (undefined1 *)0x42b839;
  System__LStrClr_40929B27(&local_8,uStack_14,puVar1);
  return;
}

