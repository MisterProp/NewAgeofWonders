// /AoW.exe : FUN_0043ffa8

void FUN_0043ffa8(int param_1,undefined4 param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  char cVar3;
  int *piVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 extraout_EDX_01;
  undefined4 extraout_EDX_02;
  undefined4 extraout_EDX_03;
  undefined4 extraout_EDX_04;
  undefined4 extraout_EDX_05;
  undefined4 *in_FS_OFFSET;
  uint *puVar8;
  undefined *puVar9;
  uint *puVar10;
  undefined4 *puVar11;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_1c;
  uint local_18;
  uint local_14;
  uint local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_2c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  puStack_30 = &LAB_00440821;
  uStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_34;
  puVar2 = &stack0xfffffffc;
  if (*(char *)(param_1 + 500) != '\x01') {
    FUN_0043fd4c(param_1,1);
    puVar2 = puStack_2c;
  }
  puStack_2c = puVar2;
  (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),0);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
  *(undefined4 *)(param_1 + 0x1ec) = param_2;
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x18c),0x15);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x19c),&DAT_00440838);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1a4),&DAT_00440838);
  (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),0);
  (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),0);
  (**(code **)(**(int **)(param_1 + 0x1d8) + 0x6c))
            (*(int **)(param_1 + 0x1d8),
             CONCAT31((int3)((uint)extraout_EDX >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x120) + 0x75)) ^ 1);
  (**(code **)(**(int **)(param_1 + 0x1d4) + 0x6c))
            (*(int **)(param_1 + 0x1d4),
             CONCAT31((int3)((uint)extraout_EDX_00 >> 8),
                      *(undefined1 *)(*(int *)(param_1 + 0x120) + 0x75)) ^ 1);
  uVar7 = *(uint *)(param_1 + 0x1ec);
  if (uVar7 < 4) {
    piVar4 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
    piVar4 = (int *)(**(code **)(*piVar4 + 0x9c))(piVar4,uVar7);
    if (piVar4 == (int *)0x0) {
      (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
    }
    else {
      (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),1);
      cVar3 = (**(code **)(*piVar4 + 0xc0))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x68),local_8);
      cVar3 = (**(code **)(*piVar4 + 0xc4))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x6c),local_8);
      cVar3 = (**(code **)(*piVar4 + 0xcc))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),local_8);
      cVar3 = (**(code **)(*piVar4 + 200))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x74),local_8);
      cVar3 = (**(code **)(*piVar4 + 0xd0))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x7c),local_8);
      cVar3 = (**(code **)(*piVar4 + 0xd4))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_8);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x70),local_8);
      uVar5 = (**(code **)(*piVar4 + 0xe8))();
      uVar5 = FUN_004541b0(*(undefined4 *)PTR_DAT_0045a420,uVar5);
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x58),uVar5);
      puVar11 = &local_8;
      uVar7 = (**(code **)(*piVar4 + 0xfc))();
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)(AoWE_TAlignmentRStr_08846F84_exref + (uVar7 & 0x7f) * 4),&local_14)
      ;
      local_10 = local_14;
      local_c._0_1_ = 0xb;
      puVar8 = &local_10;
      System_LoadResString_02614E1B(AoWE_AlignmentXRStr_04ECFCC5_exref,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      AoWE_Format_D8F48273(local_18,puVar8,0,puVar11);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x8c),local_8);
      puVar8 = &local_14;
      cVar3 = (**(code **)(*piVar4 + 0xa0))();
      SysUtils_IntToStr_0F6FDFF6((int)cVar3,&local_18);
      local_10 = local_18;
      local_c._0_1_ = 0xb;
      puVar10 = &local_10;
      System_LoadResString_02614E1B(AoWE_LevelXRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_1c);
      AoWE_Format_D8F48273(local_1c,puVar10,0,puVar8);
      puVar9 = &DAT_00440844;
      puVar8 = &local_18;
      uVar7 = local_14;
      local_10 = (**(code **)(*piVar4 + 0x9c))();
      local_10 = local_10 & 0xff;
      local_c._0_1_ = 0;
      System_LoadResString_02614E1B(AoWE_UpkeepXRStr_04ECFCC5_exref,&local_18);
      AoWE_TranslateRStr_435799AA(local_18,&local_1c);
      AoWE_Format_D8F48273(local_1c,&local_10,0,puVar8,puVar9,uVar7);
      System__LStrCatN_51F89FF7(&local_8,3);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x90),local_8);
      (**(code **)(**(int **)(*(int *)(param_1 + 0x4c) + 0x114) + 0x40))();
      AoWE_TAbilityOwner_ListAbilities_23EDC2EF
                (piVar4,*(undefined4 *)(*(int *)(param_1 + 0x4c) + 0x114));
      iVar6 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x1c),
                         *(undefined4 *)(param_1 + 0x1ec));
      if (*(char *)(iVar6 + 0xc) == '\0') {
        (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),1);
        System_LoadResString_02614E1B(AoWE_InstallRStr_04ECFCC5_exref,&local_14);
        AoWE_TranslateRStr_435799AA(local_14,&local_8);
        AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
        uVar7 = (**(code **)(**(int **)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x84))();
        (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),uVar7 ^ 1);
        System_LoadResString_02614E1B(AoWE_QueueRStr_04ECFCC5_exref,&local_14);
        AoWE_TranslateRStr_435799AA(local_14,&local_8);
        AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x120),local_8);
        (**(code **)(*piVar4 + 0xf8))(piVar4,&local_14,*piVar4,&local_8);
        local_10 = local_14;
        local_c = CONCAT31(local_c._1_3_,0xb);
        System_LoadResString_02614E1B(AoWE_InstallXRStr_8276FCC3_exref,&local_8);
        AoWE_TranslateRStr_435799AA(local_8,&local_18);
        AoWE_Format_D8F48273(local_18,&local_10,0);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa8),local_8);
        piVar1 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
        uVar5 = (**(code **)(*piVar1 + 0xe4))(piVar1,*(undefined4 *)(param_1 + 0x1ec));
        SysUtils_IntToStr_0F6FDFF6(uVar5,&local_8);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),local_8);
        piVar1 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
        uVar5 = (**(code **)(*piVar1 + 0xec))(piVar1,*(undefined4 *)(param_1 + 0x1ec));
        SysUtils_IntToStr_0F6FDFF6(uVar5,&local_8);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xb0),local_8);
        uVar5 = extraout_EDX_02;
      }
      else {
        (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),1);
        System_LoadResString_02614E1B(AoWE_ProduceRStr_04ECFCC5_exref,&local_14);
        AoWE_TranslateRStr_435799AA(local_14,&local_8);
        AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
        uVar7 = (**(code **)(**(int **)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x84))();
        (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),uVar7 ^ 1);
        System_LoadResString_02614E1B(AoWE_QueueRStr_04ECFCC5_exref,&local_14);
        AoWE_TranslateRStr_435799AA(local_14,&local_8);
        AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x120),local_8);
        (**(code **)(*piVar4 + 0xf8))(piVar4,&local_14,*piVar4,&local_8);
        local_10 = local_14;
        local_c = CONCAT31(local_c._1_3_,0xb);
        System_LoadResString_02614E1B(AoWE_ProduceXRStr_8276FCC3_exref,&local_8);
        AoWE_TranslateRStr_435799AA(local_8,&local_18);
        AoWE_Format_D8F48273(local_18,&local_10,0);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa8),local_8);
        piVar1 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
        uVar5 = (**(code **)(*piVar1 + 0xc4))(piVar1,*(undefined4 *)(param_1 + 0x1ec));
        SysUtils_IntToStr_0F6FDFF6(uVar5,&local_8);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa4),local_8);
        piVar1 = *(int **)(*(int *)(param_1 + 0x1e8) + 0x44);
        uVar5 = (**(code **)(*piVar1 + 0xd4))(piVar1,*(undefined4 *)(param_1 + 0x1ec));
        SysUtils_IntToStr_0F6FDFF6(uVar5,&local_8);
        AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xb0),local_8);
        uVar5 = extraout_EDX_01;
      }
      (**(code **)(**(int **)(param_1 + 0x1d8) + 0x6c))
                (*(int **)(param_1 + 0x1d8),
                 CONCAT31((int3)((uint)uVar5 >> 8),*(undefined1 *)(*(int *)(param_1 + 0x120) + 0x75)
                         ) ^ 1);
      (**(code **)(**(int **)(param_1 + 0x1d4) + 0x6c))
                (*(int **)(param_1 + 0x1d4),
                 CONCAT31((int3)((uint)extraout_EDX_03 >> 8),
                          *(undefined1 *)(*(int *)(param_1 + 0x120) + 0x75)) ^ 1);
      (**(code **)(*piVar4 + 0x2c))();
    }
  }
  else if (uVar7 == 4) {
    (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),1);
    iVar6 = Engine_TEObject_Create_23EDC2EF(AoWE__TProductionItem_E3617239_exref,1,0);
    *(undefined4 *)(iVar6 + 8) = 10;
    (**(code **)(**(int **)(param_1 + 0x1e8) + 0x234))(*(int **)(param_1 + 0x1e8),iVar6,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
    System_TObject_Free_23EDC2EF(iVar6);
    uVar7 = (**(code **)(**(int **)(*(int *)(param_1 + 0x1e8) + 0x44) + 0x84))();
    (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),uVar7 ^ 1);
    System_LoadResString_02614E1B(AoWE_QueueRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_8);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x120),local_8);
    (**(code **)(**(int **)(param_1 + 0x1d8) + 0x6c))
              (*(int **)(param_1 + 0x1d8),
               CONCAT31((int3)((uint)extraout_EDX_04 >> 8),
                        *(undefined1 *)(*(int *)(param_1 + 0x120) + 0x75)) ^ 1);
    (**(code **)(**(int **)(param_1 + 0x1d4) + 0x6c))
              (*(int **)(param_1 + 0x1d4),
               CONCAT31((int3)((uint)extraout_EDX_05 >> 8),
                        *(undefined1 *)(*(int *)(param_1 + 0x120) + 0x75)) ^ 1);
    cVar3 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x230))();
    if (cVar3 == '\b') {
      System_LoadResString_02614E1B(AoWE_ProduceMerchandiseTextRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_8);
      piVar4 = *(int **)(*(int *)(param_1 + 0xe0) + 0x118);
      (**(code **)(*piVar4 + 0x2c))(piVar4,local_8);
    }
    else {
      System_LoadResString_02614E1B(AoWE_GenerateExtraPowerTextRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_8);
      piVar4 = *(int **)(*(int *)(param_1 + 0xe0) + 0x118);
      (**(code **)(*piVar4 + 0x2c))(piVar4,local_8);
    }
    (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),1);
    cVar3 = (**(code **)(**(int **)(param_1 + 0x1e8) + 0x230))();
    if (cVar3 == '\b') {
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x18c),0x1b);
    }
    else {
      AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x18c),0x1c);
    }
    AoWLabel_TAOWLabel_SetGText_23EDC2EF
              (*(undefined4 *)(param_1 + 0xd8),*(undefined4 *)(*(int *)(param_1 + 0xfc) + 0x90));
  }
  else if (uVar7 == 5) {
    (**(code **)(**(int **)(param_1 + 0xfc) + 0x6c))(*(int **)(param_1 + 0xfc),1);
    System_LoadResString_02614E1B(AoWE_RazeRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_8);
    System__LStrCat_51F89FF7(&local_8,&LAB_00440850);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xfc),local_8);
    (**(code **)(**(int **)(param_1 + 0x1d8) + 0x6c))(*(int **)(param_1 + 0x1d8),0);
    (**(code **)(**(int **)(param_1 + 0x1d4) + 0x6c))(*(int **)(param_1 + 0x1d4),0);
    iVar6 = **(int **)(param_1 + 0x1e8);
    (**(code **)(iVar6 + 0x148))(*(int **)(param_1 + 0x1e8),&local_14,iVar6,&local_8);
    local_10 = local_14;
    local_c = CONCAT31(local_c._1_3_,0xb);
    puVar8 = &local_10;
    System_LoadResString_02614E1B(AoWE_ProductionPlaceXRazeRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_18);
    AoWE_Format_D8F48273(local_18,puVar8,0);
    piVar4 = *(int **)(*(int *)(param_1 + 0xe0) + 0x118);
    (**(code **)(*piVar4 + 0x2c))(piVar4,local_8);
    (**(code **)(**(int **)(param_1 + 0xd4) + 0x6c))(*(int **)(param_1 + 0xd4),1);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x18c),0x1a);
    System_LoadResString_02614E1B(AoWE_RazeRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xd8),local_8);
  }
  puVar2 = puStack_2c;
  *in_FS_OFFSET = uStack_34;
  puStack_2c = &LAB_00440828;
  puStack_30 = (undefined1 *)0x440818;
  System__LStrArrayClr_51F89FF7(&local_1c,3,puVar2);
  puStack_30 = (undefined1 *)0x440820;
  System__LStrClr_40929B27(&local_8);
  return;
}

