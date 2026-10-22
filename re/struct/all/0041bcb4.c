
void FUN_0041bcb4(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int *piVar1;
  code *pcVar2;
  int iVar3;
  int *in_FS_OFFSET;
  undefined4 uVar4;
  undefined1 *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  int iStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int *local_8;
  
  pcVar2 = AoWE_AoWHSMap_6B957A58_exref;
  puStack_2c = &stack0xfffffffc;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  local_20 = 0;
  puStack_30 = &LAB_0041c282;
  iStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = (int)&iStack_34;
  *(undefined4 *)(param_1 + 0x160) = param_4;
  *(undefined4 *)(param_1 + 0x164) = param_5;
  *(int **)(param_1 + 0x15c) = param_2;
  puVar5 = &stack0xfffffffc;
  local_8 = param_2;
  if (param_2 != (int *)0x0) {
    (**(code **)(*param_2 + 0x28))();
    puVar5 = puStack_2c;
  }
  puStack_2c = puVar5;
  AoWLabel_TAOWLabel_SetGText_23EDC2EF
            (*(undefined4 *)(param_1 + 0x78),*(undefined4 *)(*(int *)pcVar2 + 0xe4));
  (**(code **)(**(int **)(*(int *)(param_1 + 0x44) + 0x118) + 0x40))();
  piVar1 = *(int **)(*(int *)(param_1 + 0x44) + 0x118);
  (**(code **)(*piVar1 + 0x3c))(piVar1,*(undefined4 *)(*(int *)pcVar2 + 0xf0));
  System_LoadResString_02614E1B(PTR_PTR_0045a1e0,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x80),local_c);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x7c) + 0x118) + 0x40))();
  if (*(char *)(*(int *)pcVar2 + 0x14d) == '\0') {
    puVar7 = &local_c;
    System_LoadResString_02614E1B(AoWE_ConquerTheMapRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_10);
    local_18 = local_10;
    local_14 = CONCAT31(local_14._1_3_,0xb);
    puVar6 = &local_18;
    System_LoadResString_02614E1B(AoWE_ObjectiveXRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_1c);
    AoWE_Format_D8F48273(local_1c,puVar6,0,puVar7);
    piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  }
  else {
    puVar7 = &local_c;
    AoWE_TranslateRStr_435799AA
              (*(undefined4 *)
                (AoWE_TVictoryConditionRStr_7708D2DF_exref +
                (uint)*(byte *)(*(int *)pcVar2 + 0x14d) * 4),&local_10);
    local_18 = local_10;
    local_14 = CONCAT31(local_14._1_3_,0xb);
    puVar6 = &local_18;
    System_LoadResString_02614E1B(AoWE_ObjectiveXRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_1c);
    AoWE_Format_D8F48273(local_1c,puVar6,0,puVar7);
    piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  }
  if (0 < *(int *)(*(int *)pcVar2 + 0x158)) {
    puVar7 = &local_c;
    SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(*(int *)pcVar2 + 0x158),&local_10);
    local_18 = local_10;
    local_14 = CONCAT31(local_14._1_3_,0xb);
    puVar6 = &local_18;
    System_LoadResString_02614E1B(PTR_PTR_0045a4d4,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_1c);
    AoWE_Format_D8F48273(local_1c,puVar6,0,puVar7);
    piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  }
  if (*(char *)(*(int *)pcVar2 + 0x14b) == '\0') {
    puVar7 = &local_c;
    System_LoadResString_02614E1B(AoWE_DisabledRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_10);
    local_18 = local_10;
    local_14 = CONCAT31(local_14._1_3_,0xb);
    puVar6 = &local_18;
    System_LoadResString_02614E1B(AoWE_AlliedVictoryXRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_1c);
    AoWE_Format_D8F48273(local_1c,puVar6,0,puVar7);
    piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  }
  else {
    puVar7 = &local_c;
    System_LoadResString_02614E1B(AoWE_EnabledRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_10);
    local_18 = local_10;
    local_14 = CONCAT31(local_14._1_3_,0xb);
    puVar6 = &local_18;
    System_LoadResString_02614E1B(AoWE_AlliedVictoryXRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_1c);
    AoWE_Format_D8F48273(local_1c,puVar6,0,puVar7);
    piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  }
  puVar7 = &local_c;
  iVar3 = (**(code **)(**(int **)(*(int *)pcVar2 + 0x140) + 0x54))();
  SysUtils_IntToStr_0F6FDFF6(iVar3 + -1,&local_10);
  local_18 = local_10;
  local_14._0_1_ = 0xb;
  puVar6 = &local_18;
  System_LoadResString_02614E1B(AoWE_PlayersOnMapXRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_1c);
  AoWE_Format_D8F48273(local_1c,puVar6,0,puVar7);
  piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  puVar7 = &local_c;
  SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(*(int *)pcVar2 + 0xec),&local_10);
  local_18 = local_10;
  local_14._0_1_ = 0xb;
  puVar6 = &local_18;
  System_LoadResString_02614E1B(AoWE_MaximumSphereLevelXRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_1c);
  AoWE_Format_D8F48273(local_1c,puVar6,0,puVar7);
  piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  puVar7 = &local_c;
  SysUtils_IntToStr_0F6FDFF6((int)*(char *)(*(int *)pcVar2 + 0x10c),&local_10);
  local_18 = local_10;
  local_14._0_1_ = 0xb;
  puVar6 = &local_18;
  System_LoadResString_02614E1B(AoWE_MaximumPlayerHeroesXRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_1c);
  AoWE_Format_D8F48273(local_1c,puVar6,0,puVar7);
  piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  if ((*(char *)(*(int *)pcVar2 + 0x13a) == '\0') && (*(char *)(*(int *)pcVar2 + 0x139) != '\0')) {
    AoWE_TranslateRStr_435799AA
              (*(undefined4 *)
                (AoWE_TPlayerConnectionTypeRStr_80B05058_exref +
                (uint)*(byte *)(*(int *)pcVar2 + 0x13a) * 4),&local_1c);
    puVar5 = &LAB_0041c29c;
    System_LoadResString_02614E1B(AoWE_HotSeatRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_20);
    uVar4 = local_20;
    System__LStrCatN_51F89FF7(&local_10,3);
    local_18 = local_10;
    local_14 = CONCAT31(local_14._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_GametypeXRStr_04ECFCC5_exref,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_1c);
    AoWE_Format_D8F48273(local_1c,&local_18,0,uVar4);
    piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  }
  else {
    puVar7 = &local_c;
    AoWE_TranslateRStr_435799AA
              (*(undefined4 *)
                (AoWE_TPlayerConnectionTypeRStr_80B05058_exref +
                (uint)*(byte *)(*(int *)pcVar2 + 0x13a) * 4),&local_10);
    local_18 = local_10;
    local_14 = CONCAT31(local_14._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_GametypeXRStr_04ECFCC5_exref,&local_c);
    puVar5 = (undefined1 *)0x41c159;
    AoWE_TranslateRStr_435799AA(local_c,&local_1c);
    AoWE_Format_D8F48273(local_1c,&local_18,0,puVar7);
    piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  }
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)
              (AoWE_TPlayModeRStr_49FDDC44_exref + (uint)*(byte *)(*(int *)pcVar2 + 0x239) * 4),
             &local_10);
  local_18 = local_10;
  local_14 = CONCAT31(local_14._1_3_,0xb);
  System_LoadResString_02614E1B(AoWE_TurnModeXRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_1c);
  AoWE_Format_D8F48273(local_1c,&local_18,0);
  piVar1 = *(int **)(*(int *)(param_1 + 0x7c) + 0x118);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
  (**(code **)(**(int **)(param_1 + 0x84) + 0x6c))(*(int **)(param_1 + 0x84),0);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  (**(code **)(**(int **)(param_1 + 0x7c) + 0x6c))(*(int **)(param_1 + 0x7c),1);
  (**(code **)(**(int **)(param_1 + 0x68) + 0x6c))(*(int **)(param_1 + 0x68),0);
  (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))(*(int **)(param_1 + 0x70),0);
  (**(code **)(**(int **)(param_1 + 0x6c) + 0x6c))(*(int **)(param_1 + 0x6c),0);
  (**(code **)(**(int **)(param_1 + 0x88) + 0x6c))(*(int **)(param_1 + 0x88),1);
  piVar1 = *(int **)(*(int *)(param_1 + 0x88) + 0x18c);
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 0x44))(piVar1,*(undefined4 *)(*(int *)(param_1 + 0x88) + 0x184));
  }
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0x31);
  *in_FS_OFFSET = (int)&local_c;
  System__LStrArrayClr_51F89FF7(&local_20,2,puVar5,&LAB_0041c289);
  System__LStrArrayClr_51F89FF7(&local_10,2);
  return;
}

