
void FUN_00415c40(int param_1)

{
  undefined4 uVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar2;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  puStack_20 = &LAB_00415ce1;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  System_LoadResString_02614E1B(AoWE_SkillPointsRStr_04ECFCC5_exref,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  uVar1 = AoWE_THero_GetSkillPoints_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4));
  SysUtils_IntToStr_0F6FDFF6(uVar1,&local_10);
  uVar2 = local_10;
  uVar1 = AoWE_THero_GetSkillPointsMax_23EDC2EF(*(undefined4 *)(param_1 + 0x2e4));
  SysUtils_IntToStr_0F6FDFF6(uVar1,&local_14);
  uVar1 = local_14;
  System__LStrCatN_51F89FF7(&local_8,5);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x110),local_8);
  *in_FS_OFFSET = uVar1;
  System__LStrArrayClr_51F89FF7(&local_14,4,uVar2,&LAB_00415ce8);
  return;
}

