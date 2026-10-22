
void FUN_004140b0(void)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_10 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_14 = &LAB_004141aa;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  puVar2 = &stack0xfffffffc;
  if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a30c + 0xa0) + 0x75) != '\0') {
    FUN_00411f50(*(undefined4 *)PTR_DAT_0045a30c,0);
    FUN_0040e7e4(*(undefined4 *)PTR_DAT_0045a1a0);
    System_LoadResString_02614E1B(AoWE_WarningRStr_8276FCC3_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a1a0 + 0x48),local_8);
    piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,0);
    System_LoadResString_02614E1B(AoWE_YouWereEjectedByTheHostRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a1a0 + 0x80) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_8);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x70),1);
    (**(code **)(**(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44) + 0x6c))
              (*(int **)(*(int *)PTR_DAT_0045a1a0 + 0x44),1);
    puVar2 = puStack_10;
  }
  puStack_10 = puVar2;
  puVar2 = puStack_10;
  *in_FS_OFFSET = uStack_18;
  puStack_10 = &LAB_004141b1;
  puStack_14 = (undefined1 *)0x4141a9;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar2);
  return;
}

