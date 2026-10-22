
void FUN_00458e5c(void)

{
  undefined4 uVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  puStack_1c = &LAB_00458ee3;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  puVar2 = &stack0xfffffffc;
  if (*(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6e) == '\0') {
    System_LoadResString_02614E1B(AoWE_PleaseInsertTheAoWCDRetryRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    uVar1 = local_8;
    System_LoadResString_02614E1B(AoWE_PleaseInsertTheAoWCDRStr_04ECFCC5_exref,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    AoWReg_GuaranteeAoWCD_19DA5A07(local_c,uVar1);
    puVar2 = puStack_18;
  }
  puStack_18 = puVar2;
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_00458eea;
  puStack_1c = (undefined1 *)0x458ee2;
  System__LStrArrayClr_51F89FF7(&local_10,3,puVar2);
  return;
}

