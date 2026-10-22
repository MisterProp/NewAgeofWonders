
void FUN_00458dcc(void)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_3c;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined4 local_2c;
  undefined4 local_28;
  _MEMORYSTATUS local_24;
  
  puStack_34 = &stack0xfffffffc;
  local_28 = 0;
  local_2c = 0;
  puStack_38 = &LAB_00458e4d;
  uStack_3c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_3c;
  local_24.dwLength = 0x20;
  GlobalMemoryStatus(&local_24);
  if ((local_24.dwAvailVirtual >> 0x14) + (local_24.dwAvailPhys >> 0x14) < 0x32) {
    System_LoadResString_02614E1B(AoWE_LowSytemResourcesRStr_04ECFCC5_exref,&local_2c);
    AoWE_TranslateRStr_435799AA(local_2c,&local_28);
    Dialogs_ShowMessage_D425663E(local_28);
  }
  puVar1 = puStack_34;
  *in_FS_OFFSET = uStack_3c;
  puStack_34 = &LAB_00458e54;
  puStack_38 = (undefined1 *)0x458e4c;
  System__LStrArrayClr_51F89FF7(&local_2c,2,puVar1);
  return;
}

