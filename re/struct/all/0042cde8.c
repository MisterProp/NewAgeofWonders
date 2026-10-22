
void FUN_0042cde8(int param_1)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  undefined4 uVar5;
  undefined4 unaff_EBX;
  undefined4 unaff_ESI;
  int iVar6;
  undefined4 unaff_EDI;
  undefined4 *in_FS_OFFSET;
  undefined4 local_28;
  undefined1 *local_24;
  undefined1 *local_20;
  int local_10;
  int local_c;
  int local_8;
  
  local_20 = &stack0xfffffffc;
  local_10 = 4;
  do {
    local_10 = local_10 + -1;
  } while (local_10 != 0);
  local_24 = &LAB_0042d281;
  local_28 = *in_FS_OFFSET;
  *in_FS_OFFSET = &local_28;
  puVar1 = &stack0xfffffffc;
  local_8 = param_1;
  if (*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5) != -1) {
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    iVar2 = *(int *)(iVar2 + 0x54);
    (**(code **)(**(int **)(*(int *)(local_8 + 0x70) + 0x114) + 0x40))();
    (**(code **)(**(int **)(*(int *)(local_8 + 0x80) + 0x114) + 0x40))();
    (**(code **)(**(int **)(*(int *)(local_8 + 0x78) + 0x114) + 0x40))();
    iVar3 = (**(code **)(**(int **)(iVar2 + 0x3c) + 0x54))();
    if (-1 < iVar3 + -1) {
      iVar6 = 0;
      local_c = iVar3;
      do {
        piVar4 = (int *)AoWE_TEnchantmentList_GetEnchantments_23EDC2EF
                                  (*(undefined4 *)(iVar2 + 0x3c),iVar6);
        (**(code **)(*piVar4 + 0x4c))(piVar4,&local_10);
        iVar3 = local_10;
        uVar5 = AoWE_TEnchantmentList_GetEnchantments_23EDC2EF(*(undefined4 *)(iVar2 + 0x3c),iVar6);
        piVar4 = *(int **)(*(int *)(local_8 + 0x70) + 0x114);
        (**(code **)(*piVar4 + 0x38))(piVar4,iVar3,uVar5);
        piVar4 = (int *)AoWE_TEnchantmentList_GetEnchantments_23EDC2EF
                                  (*(undefined4 *)(iVar2 + 0x3c),iVar6);
        (**(code **)(*piVar4 + 0x50))(piVar4,&local_10);
        piVar4 = *(int **)(*(int *)(local_8 + 0x80) + 0x114);
        (**(code **)(*piVar4 + 0x34))(piVar4,local_10);
        iVar3 = AoWE_TEnchantmentList_GetEnchantments_23EDC2EF(*(undefined4 *)(iVar2 + 0x3c),iVar6);
        SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar3 + 0x10),&local_10);
        piVar4 = *(int **)(*(int *)(local_8 + 0x78) + 0x114);
        (**(code **)(*piVar4 + 0x34))(piVar4,local_10);
        iVar6 = iVar6 + 1;
        local_c = local_c + -1;
      } while (local_c != 0);
    }
    (**(code **)(**(int **)(*(int *)(local_8 + 0x110) + 0x114) + 0x40))();
    (**(code **)(**(int **)(*(int *)(local_8 + 0x108) + 0x114) + 0x40))();
    iVar3 = (**(code **)(**(int **)(iVar2 + 0x40) + 0x54))();
    if (-1 < iVar3 + -1) {
      iVar6 = 0;
      local_c = iVar3;
      do {
        piVar4 = (int *)AoWE_TPowerSourceList_GetItems_23EDC2EF(*(undefined4 *)(iVar2 + 0x40),iVar6)
        ;
        iVar3 = (**(code **)(*piVar4 + 0x50))();
        if (0 < iVar3) {
          piVar4 = (int *)AoWE_TPowerSourceList_GetItems_23EDC2EF
                                    (*(undefined4 *)(iVar2 + 0x40),iVar6);
          (**(code **)(*piVar4 + 0x4c))(piVar4,&local_10);
          iVar3 = local_10;
          uVar5 = AoWE_TPowerSourceList_GetItems_23EDC2EF(*(undefined4 *)(iVar2 + 0x40),iVar6);
          piVar4 = *(int **)(*(int *)(local_8 + 0x110) + 0x114);
          (**(code **)(*piVar4 + 0x38))(piVar4,iVar3,uVar5);
          piVar4 = (int *)AoWE_TPowerSourceList_GetItems_23EDC2EF
                                    (*(undefined4 *)(iVar2 + 0x40),iVar6);
          uVar5 = (**(code **)(*piVar4 + 0x50))();
          SysUtils_IntToStr_0F6FDFF6(uVar5,&local_10);
          piVar4 = *(int **)(*(int *)(local_8 + 0x108) + 0x114);
          (**(code **)(*piVar4 + 0x34))(piVar4,local_10);
        }
        iVar6 = iVar6 + 1;
        local_c = local_c + -1;
      } while (local_c != 0);
    }
    uVar5 = AoWE_TPlayerMagicControl_GetPower_23EDC2EF(iVar2);
    SysUtils_IntToStr_0F6FDFF6(uVar5,&local_10);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0xe8),local_10);
    uVar5 = AoWE_TPlayerMagicControl_GetManaIncome_23EDC2EF(iVar2);
    SysUtils_IntToStr_0F6FDFF6(uVar5,&stack0xffffffec);
    uVar5 = AoWE_TPlayerMagicControl_GetManaUpkeep_23EDC2EF(iVar2);
    SysUtils_IntToStr_0F6FDFF6(uVar5,&stack0xffffffe8);
    iVar3 = AoWE_TPlayerMagicControl_GetManaIncome_23EDC2EF(iVar2);
    iVar6 = AoWE_TPlayerMagicControl_GetManaUpkeep_23EDC2EF(iVar2);
    SysUtils_IntToStr_0F6FDFF6(iVar3 - iVar6,&stack0xffffffe4);
    System__LStrCatN_51F89FF7(&local_10,5);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0x8c),local_10);
    uVar5 = AoWE_TPlayerMagicControl_GetNetPowerToResearch_23EDC2EF(iVar2);
    SysUtils_IntToStr_0F6FDFF6(uVar5,&local_10);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0x60),local_10);
    if (*(int *)(iVar2 + 0x34) == 0) {
      (**(code **)(**(int **)(local_8 + 0xd4) + 0x6c))(*(int **)(local_8 + 0xd4),0);
    }
    else {
      iVar2 = AoWE_TSpellControl_GetSpell_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84),
                         *(int *)(iVar2 + 0x34));
      AoWLabel_TAOWLabel_SetGText_23EDC2EF
                (*(undefined4 *)(local_8 + 0xbc),*(undefined4 *)(iVar2 + 8));
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (AoWE_TMagicSphereRStr_D4FDF8A3_exref + (uint)*(byte *)(iVar2 + 0x20) * 4),
                 &stack0xffffffec);
      System_LoadResString_02614E1B(AoWE_LevelRStr_04ECFCC5_exref,&stack0xffffffe4);
      AoWE_TranslateRStr_435799AA(unaff_EDI,&stack0xffffffe8);
      SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(iVar2 + 0x21),&stack0xffffffe4);
      System__LStrCatN_51F89FF7(&local_10,5);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0xd0),local_10);
      System_LoadResString_02614E1B(AoWE_CastingCostRStr_04ECFCC5_exref,&stack0xffffffe8);
      AoWE_TranslateRStr_435799AA(unaff_ESI,&stack0xffffffec);
      SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar2 + 0x14),&stack0xffffffe8);
      System_LoadResString_02614E1B(AoWE_ManaRStr_8276FCC3_exref,&local_20);
      AoWE_TranslateRStr_435799AA(local_20,&stack0xffffffe4);
      System__LStrCatN_51F89FF7(&local_10,5);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0xd8),local_10);
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      iVar2 = AoWE_TPlayerMagicControl_GetResearchTurnsLeft_23EDC2EF(*(undefined4 *)(iVar2 + 0x54));
      if (iVar2 < 999) {
        AoWE_FmtTurnsLeftStr_0F6FDFF6(iVar2,&local_10);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0xcc),local_10);
      }
      else {
        piVar4 = &local_10;
        System_LoadResString_02614E1B(AoWE_XTurnsRStr_8276FCC3_exref,&local_10);
        AoWE_TranslateRStr_435799AA(local_10,&stack0xffffffec);
        local_28 = CONCAT31(local_28._1_3_,0x3f);
        local_24 = (undefined1 *)CONCAT31(local_24._1_3_,2);
        AoWE_Format_D8F48273(unaff_EBX,&local_28,0,piVar4);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(local_8 + 0xcc),local_10);
      }
      (**(code **)(**(int **)(local_8 + 0xd4) + 0x6c))(*(int **)(local_8 + 0xd4),1);
    }
    FUN_0042ca38(local_8);
    FUN_0042d5dc(local_8,*(undefined4 *)(*(int *)(local_8 + 0x70) + 0x120));
    puVar1 = local_20;
  }
  local_20 = puVar1;
  puVar1 = local_20;
  *(undefined1 *)(*(int *)(local_8 + 0x44) + 0xa4) = 0;
  *in_FS_OFFSET = local_28;
  local_20 = &LAB_0042d288;
  local_24 = (undefined1 *)0x42d280;
  System__LStrArrayClr_51F89FF7(&local_20,5,puVar1);
  return;
}

