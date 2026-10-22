
void FUN_0044b95c(int param_1,int *param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 in_stack_ffffffa0;
  undefined *puVar5;
  undefined4 in_stack_ffffffa8;
  undefined4 *puVar6;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_2c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  local_20 = 0;
  puStack_30 = &LAB_0044bb99;
  uStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_34;
  *(undefined4 *)(param_1 + 0x88) = param_4;
  *(undefined4 *)(param_1 + 0x8c) = param_5;
  *(int **)(param_1 + 0x90) = param_2;
  (**(code **)(*param_2 + 0x28))();
  iVar1 = *(int *)(*(int *)(param_1 + 0x90) + 0x18);
  if (*(int *)(iVar1 + 0x34) == 0) {
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),0);
    System_LoadResString_02614E1B(AoWE_NoneRStr_8276FCC3_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 100),local_8);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x6c),0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x70),0);
    piVar2 = *(int **)(*(int *)(param_1 + 0x74) + 0x118);
    (**(code **)(*piVar2 + 0x2c))(piVar2,0);
  }
  else {
    puVar6 = &local_8;
    uVar3 = WizardsTower_TWizardsTower_BuyCost_23EDC2EF
                      (*(undefined4 *)(*(int *)(param_1 + 0x90) + 0x18));
    SysUtils_IntToStr_0F6FDFF6(uVar3,&local_14);
    local_10 = local_14;
    local_c = CONCAT31(local_c._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_ForXGoldRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_18);
    AoWE_Format_D8F48273(local_18,&local_10,0,puVar6);
    System__LStrCat_51F89FF7(&local_8,&DAT_0044bbb0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x78),local_8);
    iVar4 = AoWE_TSpellControl_GetSpell_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x84),
                       *(undefined4 *)(iVar1 + 0x34));
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 100),*(undefined4 *)(iVar4 + 8));
    AoWE_TranslateRStr_435799AA
              (*(undefined4 *)
                (AoWE_TMagicSphereRStr_D4FDF8A3_exref + (uint)*(byte *)(iVar4 + 0x20) * 4),&local_14
              );
    System_LoadResString_02614E1B(AoWE_LevelRStr_04ECFCC5_exref,&local_1c);
    AoWE_TranslateRStr_435799AA(local_1c,&local_18);
    SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(iVar4 + 0x21),&local_1c);
    System__LStrCatN_51F89FF7(&local_8,5);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x6c),local_8);
    System_LoadResString_02614E1B(AoWE_CastingCostRStr_04ECFCC5_exref,&local_18);
    AoWE_TranslateRStr_435799AA(local_18,&local_14);
    SysUtils_IntToStr_0F6FDFF6(*(undefined4 *)(iVar4 + 0x14),&local_18);
    puVar5 = &DAT_0044bbc8;
    in_stack_ffffffa8 = local_18;
    System_LoadResString_02614E1B(AoWE_ManaRStr_8276FCC3_exref,&local_20);
    AoWE_TranslateRStr_435799AA(local_20,&local_1c);
    in_stack_ffffffa0 = local_1c;
    System__LStrCatN_51F89FF7(&local_8,5);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x70),local_8);
    iVar1 = **(int **)(iVar4 + 0x24);
    (**(code **)(iVar1 + 0x1c))(*(int **)(iVar4 + 0x24),&local_8,iVar1,in_stack_ffffffa0,puVar5);
    piVar2 = *(int **)(*(int *)(param_1 + 0x74) + 0x118);
    (**(code **)(*piVar2 + 0x2c))(piVar2,local_8);
  }
  (**(code **)(**(int **)(param_1 + 0x5c) + 0x6c))(*(int **)(param_1 + 0x5c),1);
  *in_FS_OFFSET = in_stack_ffffffa0;
  System__LStrArrayClr_51F89FF7(&local_20,4,in_stack_ffffffa8,&LAB_0044bba0);
  System__LStrClr_40929B27(&local_8);
  return;
}

