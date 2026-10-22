
void FUN_0042b668(void)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_c;
  int local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_18 = &LAB_0042b732;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  AoWReg_TAoWRegistry_GetLocalEmailAddress_23EDC2EF
            (*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x5c),&local_8);
  if (local_8 == 0) {
    FUN_0040e7e4(*(undefined4 *)PTR_DAT_0045a1a0);
    piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,0);
    System_LoadResString_02614E1B(AoWE_PleaseSetupEmailSettingsFirstRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70),1);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44),1);
  }
  puVar2 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0042b739;
  puStack_18 = (undefined1 *)0x42b731;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

