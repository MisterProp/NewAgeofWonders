// /AoWEPACK.dpl : 557a7f0c

void City_TCityProductionControl_ExecuteProduction(int *param_1,int param_2)

{
  undefined1 *puVar1;
  char cVar2;
  undefined1 uVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  int *piVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  undefined4 uStack_30;
  undefined1 *puStack_2c;
  undefined1 *puStack_28;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0xa7f0c  6962  City.TCityProductionControl.ExecuteProduction@23EDC2EF */
  puStack_28 = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  local_14 = 0;
  local_18 = 0;
  puStack_2c = &LAB_557a8297;
  uStack_30 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_30;
  cVar2 = AoWE_TProductionControl_ExecuteProduction_23EDC2EF(param_1,param_2);
  if (cVar2 != '\0') {
    uVar4 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(param_2 + 8));
    switch(uVar4 & 0x7f) {
    case 1:
      iVar7 = param_1[9];
      *(undefined1 *)(iVar7 + 0x1c) = *(undefined1 *)(param_2 + 0xc);
      *(undefined1 *)(iVar7 + 0x10) = 1;
      uVar3 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(param_1[9] + 0x1d) = uVar3;
      *(undefined1 *)(param_1 + 8) = 2;
      break;
    case 2:
      uVar3 = City_TCity_UpgradeTime_23EDC2EF(param_1[10]);
      iVar7 = param_1[9];
      *(undefined1 *)(iVar7 + 0x1c) = uVar3;
      *(undefined1 *)(iVar7 + 0x10) = *(undefined1 *)(param_1[9] + 0x1c);
      uVar3 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(param_1[9] + 0x1d) = uVar3;
      uVar5 = City_TCity_UpgradeCost_23EDC2EF(param_1[10]);
      *(undefined4 *)(param_1[9] + 8) = uVar5;
      cVar2 = (**(code **)(*param_1 + 0x7c))();
      iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)cVar2);
      iVar7 = *(int *)(iVar7 + 0xc4) - *(int *)(param_1[9] + 8);
      cVar2 = (**(code **)(*param_1 + 0x7c))();
      uVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)cVar2);
      AoWE_TPlayer_SetGems_23EDC2EF(uVar5,iVar7);
      *(undefined1 *)(param_1 + 8) = 2;
      break;
    case 3:
      uVar3 = City_TCity_FortifyTime_23EDC2EF(param_1[10]);
      iVar7 = param_1[9];
      *(undefined1 *)(iVar7 + 0x1c) = uVar3;
      *(undefined1 *)(iVar7 + 0x10) = *(undefined1 *)(param_1[9] + 0x1c);
      uVar3 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(param_1[9] + 0x1d) = uVar3;
      uVar5 = City_TCity_FortifyCost_23EDC2EF(param_1[10]);
      *(undefined4 *)(param_1[9] + 8) = uVar5;
      cVar2 = (**(code **)(*param_1 + 0x7c))();
      iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)cVar2);
      iVar7 = *(int *)(iVar7 + 0xc4) - *(int *)(param_1[9] + 8);
      cVar2 = (**(code **)(*param_1 + 0x7c))();
      uVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)cVar2);
      AoWE_TPlayer_SetGems_23EDC2EF(uVar5,iVar7);
      *(undefined1 *)(param_1 + 8) = 2;
      break;
    case 4:
      uVar3 = City_TCity_MigrationTime_23EDC2EF(param_1[10],*(undefined1 *)(param_2 + 0xc));
      *(undefined1 *)(param_1[9] + 0x1c) = uVar3;
      *(undefined1 *)(param_1[9] + 0x10) = *(undefined1 *)(param_1[9] + 0x1c);
      uVar3 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(param_1[9] + 0x1d) = uVar3;
      *(undefined1 *)(param_1 + 8) = 2;
      uVar5 = (**(code **)(*param_1 + 0x7c))();
      uVar6 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      iVar7 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar6,uVar5);
      iVar7 = iVar7 + -0xf;
      uVar5 = (**(code **)(*param_1 + 0x7c))();
      uVar6 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar6,uVar5,iVar7);
      break;
    case 5:
      uVar3 = City_TCity_LootTime_23EDC2EF(param_1[10]);
      iVar7 = param_1[9];
      *(undefined1 *)(iVar7 + 0x1c) = uVar3;
      *(undefined1 *)(iVar7 + 0x10) = *(undefined1 *)(param_1[9] + 0x1c);
      uVar3 = (**(code **)(*param_1 + 0x7c))();
      iVar7 = param_1[9];
      *(undefined1 *)(iVar7 + 0x1d) = uVar3;
      *(undefined4 *)(iVar7 + 8) = 0;
      uVar5 = (**(code **)(*param_1 + 0x7c))();
      uVar6 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      iVar7 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar6,uVar5);
      iVar7 = iVar7 + -0x1e;
      uVar5 = (**(code **)(*param_1 + 0x7c))();
      uVar6 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar6,uVar5,iVar7);
      piVar8 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF
                                (&EventLog__TLocationTextEventLog_4746E6CB,1,0);
      uVar3 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(piVar8 + 4) = uVar3;
      puVar13 = &local_8;
      iVar7 = *(int *)param_1[10];
      (**(code **)(iVar7 + 0x148))((int *)param_1[10],&local_14,iVar7,puVar13);
      local_10 = local_14;
      local_c = CONCAT31(local_c._1_3_,0xb);
      puVar12 = &local_10;
      System_LoadResString_02614E1B(PTR_AoWE_CityLootingRStr_04ECFCC5_558e9094,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      SysUtils_Format_D8F48273(local_18,puVar12,0,puVar13);
      System__LStrAsg_51F89FF7(piVar8 + 6,local_8);
      uVar5 = (**(code **)(*(int *)param_1[10] + 0x7c))();
      uVar6 = (**(code **)(*(int *)param_1[10] + 0x78))();
      uVar9 = (**(code **)(*(int *)param_1[10] + 0x74))();
      EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar8,uVar9,uVar6,uVar5);
      uVar5 = (**(code **)(*(int *)param_1[10] + 0x78))();
      uVar6 = (**(code **)(*(int *)param_1[10] + 0x7c))();
      uVar11 = 0;
      uVar9 = (**(code **)(*(int *)param_1[10] + 0x74))();
      uVar10 = (**(code **)(*param_1 + 0x7c))();
      EventLog_DistributeLocationEventLog_899ECAE1(piVar8,uVar10,uVar9,uVar11,uVar6,uVar5);
      (**(code **)(*piVar8 + 0x2c))();
      City_TCityOverlayHS_SetMode_23EDC2EF(*(undefined4 *)(param_1[10] + 0x60),2);
      *(undefined1 *)(param_1 + 8) = 2;
    }
  }
  puVar1 = puStack_28;
  *in_FS_OFFSET = uStack_30;
  puStack_28 = &LAB_557a829e;
  puStack_2c = (undefined1 *)0x557a828e;
  System__LStrArrayClr_51F89FF7(&local_18,2,puVar1);
  puStack_2c = (undefined1 *)0x557a8296;
  System__LStrClr_40929B27(&local_8);
  return;
}

