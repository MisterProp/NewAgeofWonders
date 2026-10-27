// /AoWEPACK.dpl : 0x55765784

void AoWE_TUnitEnchantmentAbility_GetSourceName_23EDC2EF
               (int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_8;
  
                    /* 0x65784  1993  AoWE.TUnitEnchantmentAbility.GetSourceName@23EDC2EF */
  puStack_18 = &stack0xfffffffc;
  local_8 = 0;
  puStack_1c = &LAB_5576586f;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  if (AoWE_AoWHSMap_6B957A58 == 0) {
    System_LoadResString_02614E1B(&AoWE_UnknownRStr_8276FCC3,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,param_3);
  }
  else {
    puStack_18 = &stack0xfffffffc;
    iVar2 = AoWE_TAbilityOwner_GetAbilityData_23EDC2EF(param_2,*(undefined4 *)(param_1 + 0xc));
    iVar3 = AoWE_TUnitControl_FindUnit_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0xfc),
                       *(undefined4 *)(*(int *)(iVar2 + 0x10) + 0x18));
    if (*(char *)(*(int *)(iVar2 + 0x10) + 0xc) < '\x01') {
      System_LoadResString_02614E1B(&AoWE_UnknownRStr_8276FCC3,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,param_3);
    }
    else if ((iVar3 == 0) || (*(char *)(iVar3 + 0x24) != *(char *)(*(int *)(iVar2 + 0x10) + 0xc))) {
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),
                         (int)*(char *)(*(int *)(iVar2 + 0x10) + 0xc));
      System__LStrLAsg_51F89FF7(param_3,*(undefined4 *)(*(int *)(iVar2 + 0xd4) + 0x60));
    }
    else {
      System_LoadResString_02614E1B(&AoWE_ManaRStr_8276FCC3,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,param_3);
    }
  }
  puVar1 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_55765876;
  puStack_1c = (undefined1 *)0x5576586e;
  System__LStrClr_40929B27(&local_8,uStack_20,puVar1);
  return;
}

