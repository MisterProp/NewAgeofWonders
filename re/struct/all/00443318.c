
void FUN_00443318(int param_1,int param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  byte bVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  int *in_FS_OFFSET;
  undefined4 *puVar7;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined1 *puStack_30;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int *local_8;
  
  puStack_30 = &stack0xfffffffc;
  local_8 = (int *)0x0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  local_20 = 0;
  puStack_34 = &LAB_0044376e;
  puStack_38 = (undefined1 *)*in_FS_OFFSET;
  *in_FS_OFFSET = (int)&puStack_38;
  FUN_004425d8(param_1);
  FUN_0044275c(param_1,param_2);
  System_LoadResString_02614E1B(AoWE_EnterRStr_04ECFCC5_exref,&local_14);
  AoWE_TranslateRStr_435799AA(local_14,&local_10);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x58),local_10);
  (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),1);
  *(int *)(*(int *)PTR_DAT_0045a420 + 0xb0) = param_2;
  (**(code **)(**(int **)(param_1 + 0x9c) + 0x6c))(*(int **)(param_1 + 0x9c),1);
  (**(code **)(**(int **)(param_1 + 0x150) + 0x6c))(*(int **)(param_1 + 0x150),0);
  (**(code **)(**(int **)(*(int *)(param_1 + 0x140) + 0x118) + 0x40))();
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a420 + 0xb0) + 0x148))
            (*(int **)(*(int *)PTR_DAT_0045a420 + 0xb0),&local_10);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 100),local_10);
  puVar7 = &local_10;
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)
              (AoWE_RaceNamesRStr_B11890CE_exref +
              *(char *)(*(int *)(*(int *)PTR_DAT_0045a420 + 0xb0) + 0x45) * 4),&local_14);
  local_1c = local_14;
  local_18._0_1_ = 0xb;
  System_LoadResString_02614E1B(AoWE_RaceXRStr_04ECFCC5_exref,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_20);
  AoWE_Format_D8F48273(local_20,&local_1c,0,puVar7);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0xa0),local_10);
  SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(param_2 + 0x4d),&local_10);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 300),local_10);
  SysUtils_IntToStr_0F6FDFF6(*(undefined1 *)(param_2 + 0x3c),&local_10);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x120),local_10);
  uVar4 = City_TCity_GetCivilStatus_23EDC2EF(param_2);
  AoWE_TranslateRStr_435799AA
            (*(undefined4 *)(City_TCivilStatusRStr_3B2E5A50_exref + (uVar4 & 0x7f) * 4),&local_14);
  local_1c = local_14;
  local_18 = CONCAT31(local_18._1_3_,0xb);
  System_LoadResString_02614E1B(AoWE_StatusXRStr_04ECFCC5_exref,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_20);
  AoWE_Format_D8F48273(local_20,&local_1c,0);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x118),local_10);
  iVar5 = (**(code **)(**(int **)(*(int *)(param_2 + 0x50) + 0x14) + 0x54))();
  if (iVar5 == 0) {
    piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,0);
    System_LoadResString_02614E1B(AoWE_ProducingMerchandiseRStr_04ECFCC5_exref,&local_14);
    AoWE_TranslateRStr_435799AA(local_14,&local_10);
    piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
  }
  else {
    iVar5 = AoWE_TProductionQueue_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(param_2 + 0x50) + 0x14),0);
    if (*(int *)(iVar5 + 8) == 1) {
      piVar1 = *(int **)(param_2 + 0x50);
      iVar5 = AoWE_TProductionQueue_GetItem_23EDC2EF(piVar1[5],0);
      local_8 = (int *)(**(code **)(*piVar1 + 0x9c))(piVar1,*(undefined4 *)(iVar5 + 0xc));
      (**(code **)(**(int **)(*(int *)(param_1 + 0x140) + 0x118) + 0x40))();
      System_LoadResString_02614E1B(AoWE_ProducingRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      (**(code **)(*local_8 + 0xf8))(local_8,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      (**(code **)(*local_8 + 0x2c))();
    }
    else if (*(int *)(iVar5 + 8) == 2) {
      piVar1 = *(int **)(param_2 + 0x50);
      iVar5 = AoWE_TProductionQueue_GetItem_23EDC2EF(piVar1[5],0);
      local_8 = (int *)(**(code **)(*piVar1 + 0x9c))(piVar1,*(undefined4 *)(iVar5 + 0xc));
      System_LoadResString_02614E1B(AoWE_InstallingRStr_04ECFCC5_exref,&local_14);
      AoWE_TranslateRStr_435799AA(local_14,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      (**(code **)(*local_8 + 0xf8))(local_8,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      (**(code **)(*local_8 + 0x2c))();
    }
    else {
      uVar6 = AoWE_TProductionQueue_GetItem_23EDC2EF
                        (*(undefined4 *)(*(int *)(param_2 + 0x50) + 0x14),0);
      AoWE_TProductionItem_GetNameProgressive_23EDC2EF(uVar6,&local_10);
      piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
      (**(code **)(*piVar1 + 0x2c))(piVar1,local_10);
    }
    piVar1 = *(int **)(param_2 + 0x50);
    if ((char)piVar1[8] == '\x02') {
      if (*(char *)(piVar1[9] + 0x10) == -1) {
        System_LoadResString_02614E1B(AoWE_UnlimitedRStr_04ECFCC5_exref,&local_10);
        AoWE_TranslateRStr_435799AA(local_10,&local_14);
        SysUtils_LowerCase_435799AA(local_14,&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      }
      else {
        AoWE_FmtTurnsLeftStr_0F6FDFF6
                  ((int)*(char *)(*(int *)(*(int *)(param_2 + 0x50) + 0x24) + 0x10),&local_10);
        piVar1 = *(int **)(*(int *)(param_1 + 0x140) + 0x118);
        (**(code **)(*piVar1 + 0x34))(piVar1,local_10);
      }
    }
    else if ((char)piVar1[8] == '\x01') {
      uVar6 = AoWE_TProductionQueue_GetItem_23EDC2EF(piVar1[5],0);
      (**(code **)(*piVar1 + 0xb0))(piVar1,uVar6,&local_c);
      AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x150),local_c);
      (**(code **)(**(int **)(param_1 + 0x150) + 0x6c))(*(int **)(param_1 + 0x150),1);
    }
  }
  uVar6 = AoWE_TRaceList_GetRace_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x144),
                     (int)*(char *)(param_2 + 0x45));
  bVar3 = AoWE_TRace_GetPlayerRelation_23EDC2EF(uVar6,*(undefined1 *)(param_2 + 0x30));
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x11c),bVar3 + 0xb);
  (**(code **)(**(int **)(param_1 + 0x10c) + 0x6c))(*(int **)(param_1 + 0x10c),1);
  puVar2 = puStack_38;
  *in_FS_OFFSET = (int)&local_10;
  puStack_38 = &LAB_00443775;
  System__LStrClr_40929B27(&local_20,&local_10,puVar2);
  System__LStrArrayClr_51F89FF7(&local_14,3);
  return;
}

