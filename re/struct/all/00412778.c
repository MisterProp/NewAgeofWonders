
void FUN_00412778(int param_1)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_28;
  undefined1 *puStack_24;
  undefined1 *puStack_20;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_20 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_24 = &LAB_0041284f;
  uStack_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_28;
  if (*(int *)(*(int *)(param_1 + 0xc4) + 0x148) == 0) {
    puStack_20 = &stack0xfffffffc;
    System_LoadResString_02614E1B(AoWE_NoTimerRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_c);
    SysUtils_LowerCase_435799AA(local_c,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 200),local_8);
  }
  else {
    SysUtils_IntToStr_0F6FDFF6
              (*(undefined4 *)(*(int *)(param_1 + 0xc4) + 0x148),&local_c,0,&local_8);
    local_14 = local_c;
    local_10 = CONCAT31(local_10._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_XMinutesRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_18);
    AoWE_Format_D8F48273(local_18,&local_14,0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 200),local_8);
  }
  puVar1 = puStack_20;
  *in_FS_OFFSET = uStack_28;
  puStack_20 = &LAB_00412856;
  puStack_24 = (undefined1 *)0x412841;
  System__LStrClr_40929B27(&local_18,uStack_28,puVar1);
  puStack_24 = (undefined1 *)0x41284e;
  System__LStrArrayClr_51F89FF7(&local_c,2);
  return;
}

