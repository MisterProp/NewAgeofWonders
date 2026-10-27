// /AoWEPACK.dpl : 0x557ba604

void PassiveAb_THolyFear_GetSourceName_23EDC2EF(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0xba604  7282  PassiveAb.THolyFear.GetSourceName@23EDC2EF */
  puStack_18 = &stack0xfffffffc;
  local_c = 0;
  puStack_1c = &LAB_557ba672;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  local_8 = param_3;
  iVar2 = AoWE_TAbilityOwner_GetAbilityData_23EDC2EF(param_2,*(undefined4 *)(param_1 + 0xc));
  if (*(int *)(iVar2 + 0x10) == 1) {
    System_LoadResString_02614E1B(PTR_AoWE_LifeMasteryRStr_04ECFCC5_558e9260,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,local_8);
  }
  else {
    AoWE_TAbility_GetSourceName_23EDC2EF(param_1,param_2,local_8);
  }
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_557ba679;
  puStack_1c = (undefined1 *)0x557ba671;
  System__LStrClr_40929B27(&local_c,uStack_20,puVar1);
  return;
}

