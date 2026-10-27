// /AoWEPACK.dpl : 0x55766348

void AoWE_TLeadershipAbility_GetSourceName_23EDC2EF(int param_1,int *param_2,undefined4 param_3)

{
  int iVar1;
  
                    /* 0x66348  1954  AoWE.TLeadershipAbility.GetSourceName@23EDC2EF */
  iVar1 = AoWE_TAbilityOwner_GetAbilityData_23EDC2EF(param_2,*(undefined4 *)(param_1 + 0xc));
  if ((iVar1 == 0) || (*(byte *)(iVar1 + 0x10) <= *(byte *)(iVar1 + 0xc))) {
    (**(code **)(*param_2 + 0x60))(param_2,param_3);
  }
  else {
    System__LStrLAsg_51F89FF7(param_3,*(undefined4 *)(iVar1 + 0x14));
  }
  return;
}

