
void FUN_00425780(int param_1,byte param_2)

{
  undefined1 *puVar1;
  byte bVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  undefined4 *in_FS_OFFSET;
  undefined4 *puVar6;
  undefined4 uStack_30;
  undefined1 *puStack_2c;
  undefined1 *puStack_28;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_28 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_2c = &LAB_00425a1a;
  uStack_30 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_30;
  *(undefined1 *)(param_1 + 0x284) = 0xff;
  *(byte *)(param_1 + 0x285) = param_2;
  (**(code **)(**(int **)(param_1 + 0x4c) + 0x6c))(*(int **)(param_1 + 0x4c),0);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),0);
  (**(code **)(**(int **)(param_1 + 200) + 0x6c))(*(int **)(param_1 + 200),1);
  (**(code **)(**(int **)(param_1 + 0xec) + 0x6c))(*(int **)(param_1 + 0xec),1);
  (**(code **)(**(int **)(param_1 + 0xf0) + 0x6c))(*(int **)(param_1 + 0xf0),0);
  (**(code **)(**(int **)(param_1 + 0x100) + 0x6c))(*(int **)(param_1 + 0x100),1);
  iVar5 = (int)(char)param_2;
  uVar3 = AoWE_TRaceList_GetRace_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x144),iVar5);
  bVar2 = AoWE_TRace_GetPlayerRelation_23EDC2EF
                    (uVar3,CONCAT31((int3)((uint)*(int *)AoWE_AoWHSMap_6B957A58_exref >> 8),
                                    *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)));
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x1cc),bVar2 + 0xb);
  uVar3 = AoWE_TRaceList_GetRace_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x144),iVar5);
  uVar4 = AoWE_TRace_GetPlayerRelation_23EDC2EF
                    (uVar3,CONCAT31((int3)((uint)*(int *)AoWE_AoWHSMap_6B957A58_exref >> 8),
                                    *(undefined1 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)));
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)(AoWE_TStatusRStr_8E1B04C3_exref + (uVar4 & 0x7f) * 4),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x1d4),local_8);
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)(AoWE_RaceNamesPluralRStr_B11890CE_exref + iVar5 * 4),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x104),local_8);
  if (param_2 < 4) {
    puVar6 = &local_8;
    System_LoadResString_02614E1B(AoWE_NeutralRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_14);
    local_10 = local_14;
    local_c = CONCAT31(local_c._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_AlignmentXRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_18);
    AoWE_Format_D8F48273(local_18,&local_10,0,puVar6);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x10c),local_8);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x108),0x21);
  }
  else if ((byte)(param_2 - 4) < 4) {
    puVar6 = &local_8;
    System_LoadResString_02614E1B(AoWE_GoodRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_14);
    local_10 = local_14;
    local_c = CONCAT31(local_c._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_AlignmentXRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_18);
    AoWE_Format_D8F48273(local_18,&local_10,0,puVar6);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x10c),local_8);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x108),0x1f);
  }
  else if ((byte)(param_2 - 8) < 4) {
    puVar6 = &local_8;
    System_LoadResString_02614E1B(AoWE_EvilRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_14);
    local_10 = local_14;
    local_c = CONCAT31(local_c._1_3_,0xb);
    System_LoadResString_02614E1B(AoWE_AlignmentXRStr_04ECFCC5_exref,&local_8);
    AoWE_TranslateRStr_435799AA(local_8,&local_18);
    AoWE_Format_D8F48273(local_18,&local_10,0,puVar6);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x10c),local_8);
    AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x108),0x23);
  }
  puVar1 = puStack_28;
  *in_FS_OFFSET = uStack_30;
  puStack_28 = &LAB_00425a21;
  puStack_2c = (undefined1 *)0x425a11;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar1);
  puStack_2c = (undefined1 *)0x425a19;
  System__LStrClr_40929B27(&local_8);
  return;
}

