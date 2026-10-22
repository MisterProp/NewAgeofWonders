
void FUN_0042e8e0(int param_1,int param_2,int *param_3,int param_4,int *param_5,int *param_6,
                 int *param_7,int *param_8,int *param_9)

{
  undefined4 uVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 *puVar5;
  undefined *puVar6;
  undefined4 *puVar7;
  int iVar8;
  undefined4 *puStack_54;
  undefined *in_stack_ffffffb4;
  undefined4 uStack_3c;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined4 local_24;
  uint local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int *local_8;
  
  puStack_34 = &stack0xfffffffc;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  local_20 = 0;
  local_24 = 0;
  puStack_38 = &LAB_0042ee8a;
  uStack_3c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_3c;
  local_8 = param_3;
  if (param_2 == 0) {
    (**(code **)(*param_3 + 0x6c))(param_3,0);
    goto LAB_0042ee6f;
  }
  puStack_34 = &stack0xfffffffc;
  (**(code **)(*param_3 + 0x6c))(param_3,1);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_8,*(undefined4 *)(param_2 + 8));
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)
              (AoWE_TMagicSphereRStr_D4FDF8A3_exref + (uint)*(byte *)(param_2 + 0x20) * 4),&local_14
            );
  System_LoadResString_02614E1B(AoWE_LevelRStr_04ECFCC5_exref,&local_1c);
  AoWE_TranslateRStr_435799AA(local_1c,&local_18);
  in_stack_ffffffb4 = &DAT_0042eeb0;
  SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(param_2 + 0x21),&local_1c);
  System__LStrCatN_51F89FF7(&local_10,5);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_7,local_10);
  if (*(char *)(param_1 + 0x220) == '\x02') {
    if (*(char *)(param_2 + 0xc) == '\x01') {
      System_LoadResString_02614E1B(AoWE_GlobalRStr_04ECFCC5_exref,&local_18);
      AoWE_TranslateRStr_435799AA(local_18,&local_14);
      System__LStrCatN_51F89FF7(&local_10,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_7,local_10);
    }
    else if (*(char *)(param_2 + 0xc) == '\x02') {
      System_LoadResString_02614E1B(AoWE_CombatRStr_04ECFCC5_exref,&local_18);
      AoWE_TranslateRStr_435799AA(local_18,&local_14);
      System__LStrCatN_51F89FF7(&local_10,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_7,local_10);
    }
    else if (*(char *)(param_2 + 0xc) == '\x03') {
      System_LoadResString_02614E1B(AoWE_GlobalRStr_04ECFCC5_exref,&local_18);
      AoWE_TranslateRStr_435799AA(local_18,&local_14);
      System_LoadResString_02614E1B(AoWE_CombatRStr_04ECFCC5_exref,&local_1c);
      AoWE_TranslateRStr_435799AA(local_1c,&local_18);
      System__LStrCatN_51F89FF7(&local_10,5);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_7,local_10);
    }
  }
  AoWMemo_TAOWMemo_SetFStrings_23EDC2EF(param_5,*(undefined4 *)(param_2 + 0x24));
  if (*(int *)(param_1 + 0x22c) == 0) {
    System_LoadResString_02614E1B(AoWE_CostXRStr_04ECFCC5_exref,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_14);
    local_24 = *(undefined4 *)(param_2 + 0x14);
    local_20 = local_20 & 0xffffff00;
    AoWE_Format_D8F48273(local_14,&local_24,0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,local_10);
  }
  else {
    local_24 = AoWE_THero_CastingMana_23EDC2EF(*(undefined4 *)(param_1 + 0x22c),param_2);
    local_20 = local_20 & 0xffffff00;
    puVar5 = &local_24;
    System_LoadResString_02614E1B(AoWE_CostXRStr_04ECFCC5_exref,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_14);
    AoWE_Format_D8F48273(local_14,puVar5,0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,local_10);
  }
  puStack_54 = &local_10;
  if (*(char *)(param_1 + 0x220) == '\0') {
    if ((*(byte *)(param_2 + 0xc) & 1) != 0) {
      cVar2 = AoWE_THero_Casting_23EDC2EF(*(undefined4 *)(param_1 + 0x22c));
      if (cVar2 == '\0') goto LAB_0042eb60;
    }
LAB_0042ebc9:
    (**(code **)(*param_5 + 0x70))(param_5,0x4c4c4c);
    (**(code **)(*param_8 + 0x70))(param_8,0x4c4c4c);
    (**(code **)(*param_7 + 0x70))(param_7,0x4c4c4c);
    (**(code **)(*param_6 + 0x70))(param_6,0x4c4c4c);
    (**(code **)(*param_9 + 0x6c))(param_9,0);
    *(uint *)(param_4 + 0xc) = *(uint *)(param_4 + 0xc) | 0x10;
  }
  else {
LAB_0042eb60:
    if (*(char *)(param_1 + 0x220) == '\x01') {
      if ((*(byte *)(param_2 + 0xc) & 2) != 0) {
        cVar2 = AoWE_THero_Casting_23EDC2EF(*(undefined4 *)(param_1 + 0x22c));
        if (cVar2 == '\0') {
          cVar2 = AoWE_THero_CanCastSpellInstantly_23EDC2EF
                            (*(undefined4 *)(param_1 + 0x22c),*(undefined4 *)(param_2 + 0x10),
                             *(undefined4 *)(param_2 + 0x14),&local_c);
          if (cVar2 != '\0') goto LAB_0042eb97;
        }
      }
      goto LAB_0042ebc9;
    }
LAB_0042eb97:
    if (*(char *)(param_1 + 0x220) == '\x02') {
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      if (*(int *)(*(int *)(iVar3 + 0x54) + 0x34) != 0) goto LAB_0042ebc9;
    }
    (**(code **)(*param_5 + 0x70))(param_5,0x13243c);
    (**(code **)(*param_8 + 0x70))(param_8,0x13243c);
    (**(code **)(*param_7 + 0x70))(param_7,0x5a2734);
    (**(code **)(*param_6 + 0x70))(param_6,0x5a2734);
    (**(code **)(*param_9 + 0x6c))(param_9,1);
    *(uint *)(param_4 + 0xc) = *(uint *)(param_4 + 0xc) & 0xf;
  }
  if (0 < *(int *)(param_2 + 0x1c)) {
    iVar3 = param_6[0x24];
    puVar6 = &DAT_0042eeb0;
    puVar5 = &local_14;
    System_LoadResString_02614E1B(AoWE_UpkeepXRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_18);
    local_24 = *(undefined4 *)(param_2 + 0x1c);
    local_20 = local_20 & 0xffffff00;
    AoWE_Format_D8F48273(local_18,&local_24,0,puVar5,puVar6,iVar3);
    System__LStrCatN_51F89FF7(&local_10,3);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,local_10);
  }
  if (*(byte *)(param_1 + 0x220) < 2) {
    iVar3 = AoWE_THero_CastingTurns_23EDC2EF(*(undefined4 *)(param_1 + 0x22c),param_2);
    if (iVar3 == 0) {
      System_LoadResString_02614E1B(AoWE_InstantRStr_04ECFCC5_exref,&local_10);
      AoWE_TranslateRStr_435799AA(local_10,&local_c);
    }
    else {
      puVar5 = &local_c;
      SysUtils_IntToStr_0F6FDFF6(iVar3,&local_10);
      local_24 = local_10;
      local_20 = CONCAT31(local_20._1_3_,0xb);
      puVar7 = &local_24;
      System_LoadResString_02614E1B(AoWE_TurnsXRStr_04ECFCC5_exref,&local_18);
      AoWE_TranslateRStr_435799AA(local_18,&local_14);
      AoWE_Format_D8F48273(local_14,puVar7,0,puVar5);
    }
    System__LStrCatN_51F89FF7(&local_10,3);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,local_10);
  }
  else if ((*(char *)(param_1 + 0x220) == '\x02') || (*(char *)(param_1 + 0x220) == '\x03')) {
    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    uVar1 = *(undefined4 *)(iVar3 + 0x54);
    uVar4 = AoWE_TPlayerMagicControl_GetNetPowerToResearch_23EDC2EF(uVar1);
    iVar3 = AoWE_TPlayerMagicControl_ResearchTurns_23EDC2EF
                      (uVar1,uVar4,*(undefined4 *)(param_2 + 0x18));
    if (iVar3 < 999) {
      iVar8 = param_6[0x24];
      puVar6 = &DAT_0042eeb0;
      puVar5 = &local_14;
      SysUtils_IntToStr_0F6FDFF6(iVar3,&local_18);
      local_24 = local_18;
      local_20 = CONCAT31(local_20._1_3_,0xb);
      System_LoadResString_02614E1B(AoWE_TurnsXRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_1c);
      AoWE_Format_D8F48273(local_1c,&local_24,0,puVar5,puVar6,iVar8);
      System__LStrCatN_51F89FF7(&local_10,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,local_10);
    }
    else {
      iVar3 = param_6[0x24];
      puVar6 = &DAT_0042eeb0;
      puVar5 = &local_14;
      System_LoadResString_02614E1B(AoWE_XTurnsRStr_8276FCC3_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_18);
      local_24 = CONCAT31(local_24._1_3_,0x3f);
      local_20 = CONCAT31(local_20._1_3_,2);
      AoWE_Format_D8F48273(local_18,&local_24,0,puVar5,puVar6,iVar3);
      System__LStrCatN_51F89FF7(&local_10,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(param_6,local_10);
    }
  }
LAB_0042ee6f:
  *in_FS_OFFSET = puStack_54;
  System__LStrArrayClr_51F89FF7(&local_1c,5,in_stack_ffffffb4,&LAB_0042ee91);
  return;
}

