
void FUN_0044ece8(undefined4 param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  int *in_FS_OFFSET;
  int iStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_14;
  undefined1 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_0044edcf;
  iStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = (int)&iStack_24;
  if ((*(byte *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x3c) & 4) != 0) {
    *(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x5c) + 0x11c) = 0xffffffff;
  }
  System_LoadResString_02614E1B(AoWE_DayXRStr_04ECFCC5_exref,&local_8);
  AoWE_TranslateRStr_435799AA(local_8,&local_c);
  local_10 = 0;
  local_14 = param_2;
  AoWE_Format_D8F48273(local_c,&local_14,0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a4fc + 0x5c),local_8);
  FUN_0044f52c(DAT_0045b280,*(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  FUN_0044f6fc(DAT_0045b280,*(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a0b0 + 0x44) + 0x75) != '\0') {
    FUN_00449b30(*(undefined4 *)PTR_DAT_0045a0b0);
  }
  puVar1 = puStack_20;
  *in_FS_OFFSET = (int)&local_8;
  puStack_20 = &LAB_0044edd6;
  iStack_24 = 0x44edce;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

