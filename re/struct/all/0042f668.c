
void FUN_0042f668(int param_1)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined4 unaff_EBX;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  int *piVar7;
  undefined4 *in_FS_OFFSET;
  undefined *puVar8;
  undefined *puVar9;
  undefined4 local_28;
  undefined1 *local_24;
  undefined1 *local_20;
  int local_10;
  undefined4 local_c;
  undefined3 uStack_8;
  char local_5;
  
  local_20 = &stack0xfffffffc;
  local_10 = 4;
  do {
    uStack_8 = 0;
    local_5 = '\0';
    local_10 = local_10 + -1;
  } while (local_10 != 0);
  local_24 = &LAB_004301c6;
  local_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &local_28;
  cVar2 = FUN_0042eecc(param_1);
  if (cVar2 != '\0') {
    if (*(char *)(param_1 + 0x220) == '\x01') {
      local_5 = *(char *)(*(int *)AoWTC_AoWCombatMap_2AEF5FAB_exref + 0xa5);
    }
    else {
      local_5 = *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5);
    }
    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),(int)local_5);
    local_c = *(undefined4 *)(iVar3 + 0xd4);
    iVar3 = 0;
    do {
      switch(iVar3) {
      case 0:
        piVar7 = *(int **)(param_1 + 0x198);
        break;
      case 1:
        piVar7 = *(int **)(param_1 + 0x194);
        break;
      case 2:
        piVar7 = *(int **)(param_1 + 400);
        break;
      case 3:
        piVar7 = *(int **)(param_1 + 0x188);
        break;
      case 4:
        piVar7 = *(int **)(param_1 + 0x19c);
        break;
      case 5:
        piVar7 = *(int **)(param_1 + 0x1a0);
        break;
      case 6:
        piVar7 = *(int **)(param_1 + 0x1a4);
        break;
      default:
        piVar7 = (int *)0x0;
      }
      iVar4 = AoWE_TLeader_GetSphereCount_23EDC2EF(local_c);
      if (iVar3 < iVar4) {
        (**(code **)(*piVar7 + 0x6c))(piVar7,1);
        uVar5 = AoWE_TLeader_GetSpheres_23EDC2EF(local_c,iVar3);
        switch(uVar5 & 0x7f) {
        case 1:
          (**(code **)(*piVar7 + 0xa4))(piVar7,0x1c);
          (**(code **)(*piVar7 + 0xa8))(piVar7,0x1d);
          (**(code **)(*piVar7 + 0xa0))(piVar7,0x1b);
          break;
        case 2:
          (**(code **)(*piVar7 + 0xa4))(piVar7,0x1f);
          (**(code **)(*piVar7 + 0xa8))(piVar7,0x20);
          (**(code **)(*piVar7 + 0xa0))(piVar7,0x1e);
          break;
        case 3:
          (**(code **)(*piVar7 + 0xa4))(piVar7,0x22);
          (**(code **)(*piVar7 + 0xa8))(piVar7,0x23);
          (**(code **)(*piVar7 + 0xa0))(piVar7,0x21);
          break;
        case 4:
          (**(code **)(*piVar7 + 0xa4))(piVar7,0x25);
          (**(code **)(*piVar7 + 0xa8))(piVar7,0x26);
          (**(code **)(*piVar7 + 0xa0))(piVar7,0x24);
          break;
        case 5:
          (**(code **)(*piVar7 + 0xa4))(piVar7,0x2b);
          (**(code **)(*piVar7 + 0xa8))(piVar7,0x2c);
          (**(code **)(*piVar7 + 0xa0))(piVar7,0x2a);
          break;
        case 6:
          (**(code **)(*piVar7 + 0xa4))(piVar7,0x28);
          (**(code **)(*piVar7 + 0xa8))(piVar7,0x29);
          (**(code **)(*piVar7 + 0xa0))(piVar7,0x27);
        }
      }
      else {
        (**(code **)(*piVar7 + 0x6c))(piVar7,0);
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 != 7);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0x1b0),*(undefined4 *)(*(int *)(param_1 + 0x22c) + 0x60));
    System_LoadResString_02614E1B(AoWE_ManaPoolRStr_04ECFCC5_exref,&stack0xffffffe8);
    AoWE_TranslateRStr_435799AA(unaff_ESI,&stack0xffffffec);
    iVar4 = (int)local_5;
    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
    SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(*(int *)(iVar3 + 0x54) + 0x24),&stack0xffffffe8);
    System__LStrCatN_51F89FF7(&local_10,3);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1b4),local_10);
    System_LoadResString_02614E1B(AoWE_CastingPointsRStr_04ECFCC5_exref,&stack0xffffffe8);
    AoWE_TranslateRStr_435799AA(unaff_ESI,&stack0xffffffec);
    cVar2 = (**(code **)(**(int **)(param_1 + 0x22c) + 300))();
    SysUtils_IntToStr_0F6FDFF6((int)cVar2,&stack0xffffffe8);
    cVar2 = (**(code **)(**(int **)(param_1 + 0x22c) + 0x128))();
    SysUtils_IntToStr_0F6FDFF6((int)cVar2,&stack0xffffffe4);
    System__LStrCatN_51F89FF7(&local_10,5);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x180),local_10);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0x204),*(undefined4 *)(*(int *)(param_1 + 0x1b4) + 0x90));
    System_LoadResString_02614E1B(AoWE_ResearchPointsRStr_04ECFCC5_exref,&stack0xffffffe8);
    AoWE_TranslateRStr_435799AA(unaff_ESI,&stack0xffffffec);
    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
    uVar6 = AoWE_TPlayerMagicControl_GetNetPowerToResearch_23EDC2EF(*(undefined4 *)(iVar3 + 0x54));
    SysUtils_IntToStr_0F6FDFF6(uVar6,&stack0xffffffe8);
    uVar6 = unaff_ESI;
    System__LStrCatN_51F89FF7(&local_10,3);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x200),local_10);
    if ((byte)(*(byte *)(param_1 + 0x220) - 2) < 2) {
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x118),0);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x120),0);
      (**(code **)(**(int **)(param_1 + 0x1ac) + 0x6c))(*(int **)(param_1 + 0x1ac),0);
      (**(code **)(**(int **)(param_1 + 0x210) + 0x6c))(*(int **)(param_1 + 0x210),1);
      (**(code **)(**(int **)(param_1 + 0x15c) + 0x6c))(*(int **)(param_1 + 0x15c),0);
      iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
      iVar3 = *(int *)(iVar3 + 0x54);
      if (*(int *)(iVar3 + 0x34) == 0) {
        if (*(char *)(param_1 + 0x220) == '\x03') {
          (**(code **)(**(int **)(param_1 + 0x1ec) + 0x6c))(*(int **)(param_1 + 0x1ec),1);
          (**(code **)(**(int **)(param_1 + 0x1e8) + 0x6c))(*(int **)(param_1 + 0x1e8),0);
          iVar3 = AoWE_TSpellControl_GetSpell_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84),
                             *(undefined4 *)(param_1 + 0x234));
          System_LoadResString_02614E1B(AoWE_ResearchedRStr_04ECFCC5_exref,&stack0xffffffec);
          AoWE_TranslateRStr_435799AA(unaff_EBX,&local_10);
          System__LStrCat_51F89FF7(&local_10,&DAT_004301f4);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1f0),local_10);
          AoWMemo_TAOWMemo_SetFStrings_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x1d8),*(undefined4 *)(iVar3 + 0x24));
          AoWLabel_TAOWLabel_SetGText_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x1d0),*(undefined4 *)(iVar3 + 8));
          AoWE_TranslateRStr_435799AA
                    (*(undefined4 *)
                      (AoWE_TMagicSphereRStr_D4FDF8A3_exref + (uint)*(byte *)(iVar3 + 0x20) * 4),
                     &stack0xffffffec);
          System_LoadResString_02614E1B(AoWE_LevelRStr_04ECFCC5_exref,&stack0xffffffe4);
          AoWE_TranslateRStr_435799AA(unaff_EDI,&stack0xffffffe8);
          SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(iVar3 + 0x21),&stack0xffffffe4);
          System__LStrCatN_51F89FF7(&local_10,5);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1d4),local_10);
          System_LoadResString_02614E1B(AoWE_CastingCostRStr_04ECFCC5_exref,&stack0xffffffe8);
          AoWE_TranslateRStr_435799AA(unaff_ESI,&stack0xffffffec);
          SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar3 + 0x14),&stack0xffffffe8);
          puVar9 = &DAT_0043020c;
          System_LoadResString_02614E1B(AoWE_ManaRStr_8276FCC3_exref,&local_20);
          AoWE_TranslateRStr_435799AA(local_20,&stack0xffffffe4);
          System__LStrCatN_51F89FF7(&local_10,5);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1dc),local_10);
          iVar3 = **(int **)(param_1 + 0x1e4);
          (**(code **)(iVar3 + 0x6c))(*(int **)(param_1 + 0x1e4),0,iVar3,unaff_EDI,puVar9,unaff_ESI)
          ;
          System_LoadResString_02614E1B(AoWE_ResearchRStr_04ECFCC5_exref,&stack0xffffffec);
          AoWE_TranslateRStr_435799AA(unaff_EBX,&local_10);
          AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x214),local_10);
          System_LoadResString_02614E1B(AoWE_ResearchNewSpellRStr_04ECFCC5_exref,&stack0xffffffec);
          AoWE_TranslateRStr_435799AA(unaff_EBX,&local_10);
          piVar7 = *(int **)(*(int *)(param_1 + 0x214) + 0x104);
          (**(code **)(*piVar7 + 0x2c))(piVar7,local_10);
        }
        else {
          (**(code **)(**(int **)(param_1 + 0x1ec) + 0x6c))(*(int **)(param_1 + 0x1ec),0);
          (**(code **)(**(int **)(param_1 + 0x1e8) + 0x6c))(*(int **)(param_1 + 0x1e8),1);
          System_LoadResString_02614E1B(AoWE_ResearchRStr_04ECFCC5_exref,&stack0xffffffec);
          AoWE_TranslateRStr_435799AA(unaff_EBX,&local_10);
          AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x214),local_10);
          System_LoadResString_02614E1B
                    (AoWE_CancelSpellResearchRStr_04ECFCC5_exref,&stack0xffffffec);
          AoWE_TranslateRStr_435799AA(unaff_EBX,&local_10);
          piVar7 = *(int **)(*(int *)(param_1 + 0x214) + 0x104);
          (**(code **)(*piVar7 + 0x2c))(piVar7,local_10);
        }
      }
      else {
        (**(code **)(**(int **)(param_1 + 0x1ec) + 0x6c))(*(int **)(param_1 + 0x1ec),1);
        (**(code **)(**(int **)(param_1 + 0x1e8) + 0x6c))(*(int **)(param_1 + 0x1e8),0);
        iVar4 = AoWE_TSpellControl_GetSpell_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84),
                           *(undefined4 *)(iVar3 + 0x34));
        System_LoadResString_02614E1B(AoWE_CurrentlyResearchingRStr_04ECFCC5_exref,&stack0xffffffec)
        ;
        AoWE_TranslateRStr_435799AA(unaff_EBX,&local_10);
        System__LStrCat_51F89FF7(&local_10,&DAT_004301f4);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1f0),local_10);
        AoWMemo_TAOWMemo_SetFStrings_23EDC2EF
                  (*(undefined4 *)(param_1 + 0x1d8),*(undefined4 *)(iVar4 + 0x24));
        AoWLabel_TAOWLabel_SetGText_23EDC2EF
                  (*(undefined4 *)(param_1 + 0x1d0),*(undefined4 *)(iVar4 + 8));
        AoWE_TranslateRStr_435799AA
                  (*(undefined4 *)
                    (AoWE_TMagicSphereRStr_D4FDF8A3_exref + (uint)*(byte *)(iVar4 + 0x20) * 4),
                   &stack0xffffffec);
        System_LoadResString_02614E1B(AoWE_LevelRStr_04ECFCC5_exref,&stack0xffffffe4);
        AoWE_TranslateRStr_435799AA(unaff_EDI,&stack0xffffffe8);
        SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(iVar4 + 0x21),&stack0xffffffe4);
        System__LStrCatN_51F89FF7(&local_10,5);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1d4),local_10);
        System_LoadResString_02614E1B(AoWE_CastingCostRStr_04ECFCC5_exref,&stack0xffffffe8);
        AoWE_TranslateRStr_435799AA(unaff_ESI,&stack0xffffffec);
        uVar6 = AoWE_THero_CastingMana_23EDC2EF(*(undefined4 *)(param_1 + 0x22c),iVar4);
        SysUtils_IntToStr_0F6FDFF6(uVar6,&stack0xffffffe8);
        System_LoadResString_02614E1B(AoWE_ManaRStr_8276FCC3_exref,&local_20);
        AoWE_TranslateRStr_435799AA(local_20,&stack0xffffffe4);
        System__LStrCatN_51F89FF7(&local_10,5);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1dc),local_10);
        iVar4 = AoWE_TPlayerMagicControl_GetResearchTurnsLeft_23EDC2EF(iVar3);
        if (iVar4 < 999) {
          uVar6 = AoWE_TPlayerMagicControl_GetResearchTurnsLeft_23EDC2EF(iVar3);
          AoWE_FmtTurnsLeftStr_0F6FDFF6(uVar6,&local_10);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1e4),local_10);
        }
        else {
          piVar7 = &local_10;
          System_LoadResString_02614E1B(AoWE_XTurnsRStr_8276FCC3_exref,&local_10);
          AoWE_TranslateRStr_435799AA(local_10,&stack0xffffffec);
          local_28 = CONCAT31(local_28._1_3_,0x3f);
          local_24 = (undefined1 *)CONCAT31(local_24._1_3_,2);
          AoWE_Format_D8F48273(unaff_EBX,&local_28,0,piVar7);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1e4),local_10);
        }
        System_LoadResString_02614E1B(AoWE_CancelRStr_8276FCC3_exref,&stack0xffffffec);
        AoWE_TranslateRStr_435799AA(unaff_EBX,&local_10);
        AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x214),local_10);
        (**(code **)(**(int **)(param_1 + 0x1e4) + 0x6c))(*(int **)(param_1 + 0x1e4),1);
      }
    }
    else if (*(byte *)(param_1 + 0x220) < 2) {
      cVar2 = (**(code **)(**(int **)(param_1 + 0x22c) + 300))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar2,&local_10);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x118),local_10);
      cVar2 = (**(code **)(**(int **)(param_1 + 0x22c) + 0x128))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar2,&local_10);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x120),local_10);
      (**(code **)(**(int **)(param_1 + 0x1ac) + 0x6c))(*(int **)(param_1 + 0x1ac),1);
      (**(code **)(**(int **)(param_1 + 0x210) + 0x6c))(*(int **)(param_1 + 0x210),0);
      (**(code **)(**(int **)(param_1 + 0x1cc) + 0x6c))(*(int **)(param_1 + 0x1cc),0);
      cVar2 = AoWE_THero_Casting_23EDC2EF(*(undefined4 *)(param_1 + 0x22c));
      if (cVar2 == '\0') {
        (**(code **)(**(int **)(param_1 + 0x160) + 0x6c))(*(int **)(param_1 + 0x160),0);
        (**(code **)(**(int **)(param_1 + 0x1c8) + 0x6c))(*(int **)(param_1 + 0x1c8),1);
      }
      else {
        (**(code **)(**(int **)(param_1 + 0x1c8) + 0x6c))(*(int **)(param_1 + 0x1c8),0);
        iVar3 = AoWE_TSpellControl_GetSpell_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84),
                           *(undefined4 *)(*(int *)(param_1 + 0x22c) + 0x84));
        AoWLabel_TAOWLabel_SetGText_23EDC2EF
                  (*(undefined4 *)(param_1 + 0x164),*(undefined4 *)(iVar3 + 8));
        AoWE_TranslateRStr_435799AA
                  (*(undefined4 *)
                    (AoWE_TMagicSphereRStr_D4FDF8A3_exref + (uint)*(byte *)(iVar3 + 0x20) * 4),
                   &stack0xffffffec);
        System_LoadResString_02614E1B(AoWE_LevelRStr_04ECFCC5_exref,&stack0xffffffe4);
        AoWE_TranslateRStr_435799AA(unaff_EDI,&stack0xffffffe8);
        SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(iVar3 + 0x21),&stack0xffffffe4);
        System__LStrCatN_51F89FF7(&local_10,5);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x168),local_10);
        System_LoadResString_02614E1B(AoWE_CastingCostRStr_04ECFCC5_exref,&stack0xffffffe8);
        AoWE_TranslateRStr_435799AA(unaff_ESI,&stack0xffffffec);
        SysUtils_IntToStr_0F6FDFF6
                  (*(undefined4 *)(*(int *)(param_1 + 0x22c) + 0x8c),&stack0xffffffe8);
        System_LoadResString_02614E1B(AoWE_ManaRStr_8276FCC3_exref,&local_20);
        AoWE_TranslateRStr_435799AA(local_20,&stack0xffffffe4);
        System__LStrCatN_51F89FF7(&local_10,5);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x170),local_10);
        AoWMemo_TAOWMemo_SetFStrings_23EDC2EF
                  (*(undefined4 *)(param_1 + 0x16c),*(undefined4 *)(iVar3 + 0x24));
        puVar8 = &DAT_00430218;
        uVar6 = AoWE_THero_CastingTurnsLeft_23EDC2EF(*(undefined4 *)(param_1 + 0x22c));
        AoWE_FmtTurnsLeftStr_0F6FDFF6(uVar6,&stack0xffffffec);
        puVar9 = &DAT_00430224;
        uVar6 = unaff_EBX;
        System__LStrCatN_51F89FF7(&local_10,3);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x17c),local_10);
        iVar3 = **(int **)(param_1 + 0x160);
        (**(code **)(iVar3 + 0x6c))(*(int **)(param_1 + 0x160),1,iVar3,puVar9,uVar6,puVar8);
        cVar2 = AoWE_THero_CastingReady_23EDC2EF(*(undefined4 *)(param_1 + 0x22c));
        if (cVar2 == '\0') {
          (**(code **)(**(int **)(param_1 + 0x184) + 0x6c))(*(int **)(param_1 + 0x184),0);
          uVar6 = AoWE_THero_CastingTurnsLeft_23EDC2EF(*(undefined4 *)(param_1 + 0x22c));
          AoWE_FmtTurnsLeftStr_0F6FDFF6(uVar6,&stack0xffffffec);
          System__LStrCatN_51F89FF7(&local_10,3);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x17c),local_10);
        }
        else {
          (**(code **)(**(int **)(param_1 + 0x184) + 0x6c))(*(int **)(param_1 + 0x184),1);
          System_LoadResString_02614E1B(AoWE_CastingReadyRStr_04ECFCC5_exref,&stack0xffffffec);
          AoWE_TranslateRStr_435799AA(unaff_EBX,&local_10);
          AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x17c),local_10);
        }
      }
    }
    else {
      iVar3 = **(int **)(param_1 + 0x160);
      (**(code **)(iVar3 + 0x6c))(*(int **)(param_1 + 0x160),0,iVar3,uVar6);
      (**(code **)(**(int **)(param_1 + 0x1c8) + 0x6c))(*(int **)(param_1 + 0x1c8),1);
    }
  }
  puVar1 = local_20;
  *in_FS_OFFSET = local_28;
  local_20 = &LAB_004301cd;
  local_24 = (undefined1 *)0x4301c5;
  System__LStrArrayClr_51F89FF7(&local_20,5,puVar1);
  return;
}

