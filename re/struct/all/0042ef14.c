
void FUN_0042ef14(int param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 in_stack_ffffff10;
  undefined4 in_stack_ffffff18;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 uStack_30;
  undefined1 *puStack_2c;
  undefined1 *puStack_28;
  undefined1 local_1c [4];
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined1 local_c;
  undefined4 local_8;
  
  puStack_28 = &stack0xfffffffc;
  local_8 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_2c = &LAB_0042f657;
  uStack_30 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_30;
  cVar1 = FUN_0042eecc(param_1);
  if (cVar1 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x1c4) + 0x6c))(*(int **)(param_1 + 0x1c4),0);
  }
  else {
    cVar1 = *(char *)(param_1 + 0x220);
    if (cVar1 == '\0') {
      AoWE_TSpellList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      uVar4 = *(undefined4 *)(iVar2 + 0x54);
      System__SetElem_51F89FF7(local_1c,*(undefined1 *)(param_1 + 0x222),1);
      AoWE_TPlayerMagicControl_ListSpells_23EDC2EF(uVar4,param_1 + 0x224,local_1c[0]);
      AoWE_TSpellList_SortOnLevel_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
      puVar6 = &local_8;
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_TSpellTypeRStr_7B8EDA83_exref + (uint)*(byte *)(param_1 + 0x222) * 4),
                 &local_14);
      local_10 = local_14;
      local_c = 0xb;
      puVar5 = &local_10;
      System_LoadResString_02614E1B(AoWE_CastXSpellRStr_04ECFCC5_exref,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      AoWE_Format_D8F48273(local_18,puVar5,0,puVar6);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x44),local_8);
      puVar6 = &local_8;
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_TSpellTypeRStr_7B8EDA83_exref + (uint)*(byte *)(param_1 + 0x222) * 4),
                 &local_14);
      local_10 = local_14;
      local_c = 0xb;
      puVar5 = &local_10;
      System_LoadResString_02614E1B(AoWE_CastXSpellRStr_04ECFCC5_exref,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      AoWE_Format_D8F48273(local_18,puVar5,0,puVar6);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xac),local_8);
    }
    else if (cVar1 == '\x01') {
      AoWE_TSpellList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWTC_AoWCombatMap_2AEF5FAB_exref + 0xa5));
      uVar4 = *(undefined4 *)(iVar2 + 0x54);
      System__SetElem_51F89FF7(local_1c,*(undefined1 *)(param_1 + 0x222),1);
      AoWE_TPlayerMagicControl_ListSpells_23EDC2EF(uVar4,param_1 + 0x224,local_1c[0]);
      AoWE_TSpellList_SortOnLevel_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
      puVar6 = &local_8;
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_TSpellTypeRStr_7B8EDA83_exref + (uint)*(byte *)(param_1 + 0x222) * 4),
                 &local_14);
      local_10 = local_14;
      local_c = 0xb;
      puVar5 = &local_10;
      System_LoadResString_02614E1B(AoWE_CastXSpellRStr_04ECFCC5_exref,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      AoWE_Format_D8F48273(local_18,puVar5,0,puVar6);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x44),local_8);
      puVar6 = &local_8;
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_TSpellTypeRStr_7B8EDA83_exref + (uint)*(byte *)(param_1 + 0x222) * 4),
                 &local_14);
      local_10 = local_14;
      local_c = 0xb;
      puVar5 = &local_10;
      System_LoadResString_02614E1B(AoWE_CastXSpellRStr_04ECFCC5_exref,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      AoWE_Format_D8F48273(local_18,puVar5,0,puVar6);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xac),local_8);
    }
    else if ((byte)(cVar1 - 2U) < 2) {
      AoWE_TSpellList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      AoWE_TPlayerMagicControl_ListResearchSpells_23EDC2EF
                (*(undefined4 *)(iVar2 + 0x54),param_1 + 0x224,7);
      AoWE_TSpellList_SortOnLevel_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
      puVar6 = &local_8;
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_TSpellTypeRStr_7B8EDA83_exref + (uint)*(byte *)(param_1 + 0x222) * 4),
                 &local_14);
      local_10 = local_14;
      local_c = 0xb;
      puVar5 = &local_10;
      System_LoadResString_02614E1B(AoWE_ResearchXspellRStr_04ECFCC5_exref,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      AoWE_Format_D8F48273(local_18,puVar5,0,puVar6);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x44),local_8);
      puVar6 = &local_8;
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_TSpellTypeRStr_7B8EDA83_exref + (uint)*(byte *)(param_1 + 0x222) * 4),
                 &local_14);
      local_10 = local_14;
      local_c = 0xb;
      puVar5 = &local_10;
      System_LoadResString_02614E1B(AoWE_ResearchXspellRStr_04ECFCC5_exref,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      AoWE_Format_D8F48273(local_18,puVar5,0,puVar6);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xac),local_8);
      System_LoadResString_02614E1B(AoWE_ResearchSpellRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x44),local_8);
      System_LoadResString_02614E1B(AoWE_ResearchSpellRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xac),local_8);
    }
    iVar2 = AoWE_TSpellList_GetCount_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
    iVar2 = iVar2 + -1;
    if (-1 < iVar2) {
      do {
        iVar3 = AoWE_TSpellList_GetSpell_23EDC2EF(*(undefined4 *)(param_1 + 0x224),iVar2);
        if (99 < *(int *)(iVar3 + 0x10)) {
          iVar3 = AoWE_TSpellList_GetSpell_23EDC2EF(*(undefined4 *)(param_1 + 0x224),iVar2);
          if (AoWTC_SpellTypes_57EA1744_exref[*(int *)(iVar3 + 0x10)] == (code)0x0) {
            AoWE_TSpellList_Delete_23EDC2EF(*(undefined4 *)(param_1 + 0x224),iVar2);
          }
        }
        iVar2 = iVar2 + -1;
      } while (iVar2 != -1);
    }
    uVar4 = AoWE_TSpellList_GetSpell_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x224),*(int *)(param_1 + 0x228) << 3);
    FUN_0042e8e0(param_1,uVar4,*(undefined4 *)(param_1 + 0x4c),*(undefined4 *)(param_1 + 0x60),
                 *(undefined4 *)(param_1 + 0x58),*(undefined4 *)(param_1 + 0x5c),
                 *(undefined4 *)(param_1 + 0x54),*(undefined4 *)(param_1 + 0x50),
                 *(undefined4 *)(param_1 + 0x114));
    uVar4 = AoWE_TSpellList_GetSpell_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x224),*(int *)(param_1 + 0x228) * 8 + 1);
    FUN_0042e8e0(param_1,uVar4,*(undefined4 *)(param_1 + 100),*(undefined4 *)(param_1 + 0x78),
                 *(undefined4 *)(param_1 + 0x70),*(undefined4 *)(param_1 + 0x74),
                 *(undefined4 *)(param_1 + 0x6c),*(undefined4 *)(param_1 + 0x68),
                 *(undefined4 *)(param_1 + 0x140));
    uVar4 = AoWE_TSpellList_GetSpell_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x224),*(int *)(param_1 + 0x228) * 8 + 2);
    FUN_0042e8e0(param_1,uVar4,*(undefined4 *)(param_1 + 0x7c),*(undefined4 *)(param_1 + 0x90),
                 *(undefined4 *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x8c),
                 *(undefined4 *)(param_1 + 0x84),*(undefined4 *)(param_1 + 0x80),
                 *(undefined4 *)(param_1 + 0x144));
    uVar4 = AoWE_TSpellList_GetSpell_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x224),*(int *)(param_1 + 0x228) * 8 + 3);
    FUN_0042e8e0(param_1,uVar4,*(undefined4 *)(param_1 + 0x94),*(undefined4 *)(param_1 + 0xa8),
                 *(undefined4 *)(param_1 + 0xa0),*(undefined4 *)(param_1 + 0xa4),
                 *(undefined4 *)(param_1 + 0x9c),*(undefined4 *)(param_1 + 0x98),
                 *(undefined4 *)(param_1 + 0x148));
    uVar4 = AoWE_TSpellList_GetSpell_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x224),*(int *)(param_1 + 0x228) * 8 + 4);
    FUN_0042e8e0(param_1,uVar4,*(undefined4 *)(param_1 + 0xb4),*(undefined4 *)(param_1 + 200),
                 *(undefined4 *)(param_1 + 0xc0),*(undefined4 *)(param_1 + 0xc4),
                 *(undefined4 *)(param_1 + 0xbc),*(undefined4 *)(param_1 + 0xb8),
                 *(undefined4 *)(param_1 + 0x14c));
    uVar4 = AoWE_TSpellList_GetSpell_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x224),*(int *)(param_1 + 0x228) * 8 + 5);
    FUN_0042e8e0(param_1,uVar4,*(undefined4 *)(param_1 + 0xcc),*(undefined4 *)(param_1 + 0xe0),
                 *(undefined4 *)(param_1 + 0xd8),*(undefined4 *)(param_1 + 0xdc),
                 *(undefined4 *)(param_1 + 0xd4),*(undefined4 *)(param_1 + 0xd0),
                 *(undefined4 *)(param_1 + 0x150));
    uVar4 = AoWE_TSpellList_GetSpell_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x224),*(int *)(param_1 + 0x228) * 8 + 6);
    FUN_0042e8e0(param_1,uVar4,*(undefined4 *)(param_1 + 0xe4),*(undefined4 *)(param_1 + 0xf8),
                 *(undefined4 *)(param_1 + 0xf0),*(undefined4 *)(param_1 + 0xf4),
                 *(undefined4 *)(param_1 + 0xec),*(undefined4 *)(param_1 + 0xe8),
                 *(undefined4 *)(param_1 + 0x154));
    uVar4 = AoWE_TSpellList_GetSpell_23EDC2EF
                      (*(undefined4 *)(param_1 + 0x224),*(int *)(param_1 + 0x228) * 8 + 7);
    in_stack_ffffff18 = *(undefined4 *)(param_1 + 0x10c);
    in_stack_ffffff10 = *(undefined4 *)(param_1 + 0x110);
    FUN_0042e8e0(param_1,uVar4,*(undefined4 *)(param_1 + 0xfc),in_stack_ffffff10,
                 *(undefined4 *)(param_1 + 0x108),in_stack_ffffff18,*(undefined4 *)(param_1 + 0x104)
                 ,*(undefined4 *)(param_1 + 0x100),*(undefined4 *)(param_1 + 0x158));
    if (*(char *)(param_1 + 0x220) == '\0') {
      Engine_TDoubleItemList_SetCount_23EDC2EF
                (*(undefined4 *)(param_1 + 0x244),
                 *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) + 1);
      *(uint *)(*(int *)(*(int *)(param_1 + 0x244) + 4) +
               *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) * 8) =
           (uint)*(byte *)(param_1 + 0x222);
      *(undefined4 *)
       (*(int *)(*(int *)(param_1 + 0x244) + 4) + 4 +
       *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) * 8) =
           *(undefined4 *)(param_1 + 0x228);
    }
    if (*(char *)(param_1 + 0x221) == '\x01') {
      iVar2 = AoWE_TSpellList_GetCount_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
      iVar3 = *(int *)(param_1 + 0x228) + 1;
      (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))
                (*(int **)(param_1 + 0x138),
                 CONCAT31((int3)((uint)(iVar3 * 8) >> 8),0 < iVar2 + iVar3 * -8));
      (**(code **)(**(int **)(param_1 + 0x134) + 0x6c))
                (*(int **)(param_1 + 0x134),*(int *)(param_1 + 0x228) != 0);
    }
  }
  *in_FS_OFFSET = in_stack_ffffff10;
  System__LStrArrayClr_51F89FF7(&local_18,2,in_stack_ffffff18,&LAB_0042f65e);
  System__LStrClr_40929B27(&local_8);
  return;
}

