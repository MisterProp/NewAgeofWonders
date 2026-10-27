// /AoWEPACK.dpl : 557a82a8

void City_TCityProductionControl_NewTurn_23EDC2EF(int *param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  undefined1 uVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  int *piVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 extraout_EDX;
  undefined4 *in_FS_OFFSET;
  undefined4 *puVar10;
  undefined4 uStack_44;
  undefined1 *puStack_40;
  undefined1 *puStack_3c;
  undefined4 local_2c;
  undefined4 local_28;
  undefined1 local_24;
  undefined4 local_20;
  undefined1 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined1 local_c;
  undefined4 local_8;
  
                    /* 0xa82a8  6961  City.TCityProductionControl.NewTurn@23EDC2EF */
  puStack_3c = &stack0xfffffffc;
  local_8 = 0;
  local_14 = 0;
  local_18 = 0;
  local_2c = 0;
  puStack_40 = &LAB_557a88ab;
  uStack_44 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_44;
  AoWE_TProductionControl_NewTurn_23EDC2EF(param_1,param_2);
  if ((((char)param_1[8] == '\x02') && ((char)param_2 == *(char *)(param_1[9] + 0x1d))) &&
     (*(char *)(param_1[9] + 0x10) == '\0')) {
    iVar4 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
    uVar5 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(iVar4 + 8));
    switch(uVar5 & 0x7f) {
    case 1:
      AoWE_TProductionControl_ProductionDone_23EDC2EF(param_1);
      break;
    case 2:
      AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
      City_TCity_SetUpgradeLevel_23EDC2EF
                (param_1[10],
                 CONCAT31((int3)((uint)extraout_EDX >> 8),*(undefined1 *)(param_1[10] + 0x4d)) + 1);
      uVar7 = (**(code **)(*param_1 + 0x7c))();
      uVar8 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      iVar4 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar8,uVar7);
      iVar4 = iVar4 + 5;
      uVar7 = (**(code **)(*param_1 + 0x7c))();
      uVar8 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar8,uVar7,iVar4);
      AoWE_TProductionControl_ProductionDone_23EDC2EF(param_1);
      piVar6 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF
                                (&City__TCityProductionEventLog_56E7A6ED,1,0);
      uVar2 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(piVar6 + 4) = uVar2;
      puVar10 = &local_8;
      iVar4 = *(int *)param_1[10];
      (**(code **)(iVar4 + 0x148))((int *)param_1[10],&local_14,iVar4,puVar10);
      local_10 = local_14;
      local_c = 0xb;
      System_LoadResString_02614E1B(PTR_AoWE_XUpgradedRStr_04ECFCC5_558e94b8,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      SysUtils_Format_D8F48273(local_18,&local_10,0,puVar10);
      System__LStrAsg_51F89FF7(piVar6 + 6,local_8);
      uVar7 = (**(code **)(*(int *)param_1[10] + 0x7c))();
      uVar8 = (**(code **)(*(int *)param_1[10] + 0x78))();
      uVar9 = (**(code **)(*(int *)param_1[10] + 0x74))();
      EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar6,uVar9,uVar8,uVar7);
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)(char)piVar6[4]);
      EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar4 + 0xd8),piVar6);
      (**(code **)(*piVar6 + 0x2c))();
      AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
      break;
    case 3:
      AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
      iVar4 = param_1[10];
      if (*(char *)(iVar4 + 0x4c) == '\0') {
        *(undefined1 *)(iVar4 + 0x4c) = 1;
      }
      else if (*(char *)(iVar4 + 0x4c) == '\x01') {
        *(undefined1 *)(iVar4 + 0x4c) = 2;
      }
      piVar6 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF
                                (&City__TCityProductionEventLog_56E7A6ED,1,0);
      uVar2 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(piVar6 + 4) = uVar2;
      puVar10 = &local_8;
      iVar4 = *(int *)param_1[10];
      (**(code **)(iVar4 + 0x148))((int *)param_1[10],&local_14,iVar4,puVar10);
      local_10 = local_14;
      local_c = 0xb;
      System_LoadResString_02614E1B(PTR_AoWE_CityFortifiedRStr_04ECFCC5_558e9258,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_18);
      SysUtils_Format_D8F48273(local_18,&local_10,0,puVar10);
      System__LStrAsg_51F89FF7(piVar6 + 6,local_8);
      uVar7 = (**(code **)(*(int *)param_1[10] + 0x7c))();
      uVar8 = (**(code **)(*(int *)param_1[10] + 0x78))();
      uVar9 = (**(code **)(*(int *)param_1[10] + 0x74))();
      EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar6,uVar9,uVar8,uVar7);
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)(char)piVar6[4]);
      EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar4 + 0xd8),piVar6);
      (**(code **)(*piVar6 + 0x2c))();
      City_TCity_UpdateImages_23EDC2EF(param_1[10]);
      uVar7 = (**(code **)(*param_1 + 0x7c))();
      uVar8 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      iVar4 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar8,uVar7);
      iVar4 = iVar4 + 5;
      uVar7 = (**(code **)(*param_1 + 0x7c))();
      uVar8 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar8,uVar7,iVar4);
      AoWE_TProductionControl_ProductionDone_23EDC2EF(param_1);
      AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
      break;
    case 4:
      AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
      piVar6 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF
                                (&City__TCityProductionEventLog_56E7A6ED,1,0);
      uVar2 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(piVar6 + 4) = uVar2;
      puVar10 = &local_8;
      iVar4 = *(int *)param_1[10];
      (**(code **)(iVar4 + 0x148))((int *)param_1[10],&local_14,iVar4,puVar10);
      local_28 = local_14;
      local_24 = 0xb;
      iVar4 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
      AoWE_TranslateRStr_435799AA
                (*(undefined4 *)
                  (PTR_AoWE_RaceNamesPluralRStr_B11890CE_558e9480 + *(int *)(iVar4 + 0xc) * 4),
                 &local_18);
      local_20 = local_18;
      local_1c = 0xb;
      System_LoadResString_02614E1B(PTR_AoWE_CityMigratedToRStr_04ECFCC5_558e8ed4,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_2c);
      SysUtils_Format_D8F48273(local_2c,&local_28,1,puVar10);
      System__LStrAsg_51F89FF7(piVar6 + 6,local_8);
      uVar7 = (**(code **)(*(int *)param_1[10] + 0x7c))();
      uVar8 = (**(code **)(*(int *)param_1[10] + 0x78))();
      uVar9 = (**(code **)(*(int *)param_1[10] + 0x74))();
      EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar6,uVar9,uVar8,uVar7);
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)(char)piVar6[4]);
      EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar4 + 0xd8),piVar6);
      (**(code **)(*piVar6 + 0x2c))();
      iVar4 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
      (**(code **)(*(int *)param_1[10] + 0x244))((int *)param_1[10],*(undefined1 *)(iVar4 + 0xc));
      uVar7 = (**(code **)(*param_1 + 0x7c))();
      uVar8 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      iVar4 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar8,uVar7);
      iVar4 = iVar4 + 10;
      uVar7 = (**(code **)(*param_1 + 0x7c))();
      uVar8 = AoWE_TRaceList_GetRace_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                         (int)*(char *)(param_1[10] + 0x45));
      AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar8,uVar7,iVar4);
      (**(code **)(*(int *)param_1[7] + 0x30))();
      City_TCity_SetUpgradeLevel_23EDC2EF(param_1[10],*(undefined1 *)(param_1[10] + 0x4d));
      AoWE_TProductionControl_ProductionDone_23EDC2EF(param_1);
      AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
      break;
    case 5:
      AoWE_TProductionControl_BeginUpdate_23EDC2EF(param_1);
      piVar6 = (int *)EventLog_TLocationTextEventLog_Create_23EDC2EF
                                (&City__TCityProductionEventLog_56E7A6ED,1,0);
      uVar2 = (**(code **)(*param_1 + 0x7c))();
      *(undefined1 *)(piVar6 + 4) = uVar2;
      puVar10 = &local_8;
      iVar4 = *(int *)param_1[10];
      (**(code **)(iVar4 + 0x148))((int *)param_1[10],&local_14,iVar4,puVar10);
      local_28 = local_14;
      local_24 = 0xb;
      SysUtils_IntToStr_0F6FDFF6((uint)*(byte *)(param_1[10] + 0x3c) * 3,&local_18);
      local_20 = local_18;
      local_1c = 0xb;
      System_LoadResString_02614E1B(PTR_AoWE_CityLootedRStr_04ECFCC5_558e9508,&local_8);
      AoWE_TranslateRStr_435799AA(local_8,&local_2c);
      SysUtils_Format_D8F48273(local_2c,&local_28,1,puVar10);
      System__LStrAsg_51F89FF7(piVar6 + 6,local_8);
      uVar7 = (**(code **)(*(int *)param_1[10] + 0x7c))();
      uVar8 = (**(code **)(*(int *)param_1[10] + 0x78))();
      uVar9 = (**(code **)(*(int *)param_1[10] + 0x74))();
      EventLog_TLocationTextEventLog_SetLocation_23EDC2EF(piVar6,uVar9,uVar8,uVar7);
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)(char)piVar6[4]);
      EventLog_TEventLogbook_AddEvent_23EDC2EF(*(undefined4 *)(iVar4 + 0xd8),piVar6);
      (**(code **)(*piVar6 + 0x2c))();
      cVar3 = (**(code **)(*param_1 + 0x7c))();
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)cVar3);
      iVar4 = *(int *)(iVar4 + 0xc4) + (uint)*(byte *)(param_1[10] + 0x3c) * 3;
      cVar3 = (**(code **)(*param_1 + 0x7c))();
      uVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)cVar3);
      AoWE_TPlayer_SetGems_23EDC2EF(uVar7,iVar4);
      City_TCityOverlayHS_SetMode_23EDC2EF(*(undefined4 *)(param_1[10] + 0x60),0);
      (**(code **)(*(int *)param_1[10] + 0x14c))((int *)param_1[10],1);
      AoWE_TProductionControl_EndUpdate_23EDC2EF(param_1);
    }
  }
  puVar1 = puStack_3c;
  *in_FS_OFFSET = uStack_44;
  puStack_3c = &LAB_557a88b2;
  puStack_40 = (undefined1 *)0x557a8895;
  System__LStrClr_40929B27(&local_2c,uStack_44,puVar1);
  puStack_40 = (undefined1 *)0x557a88a2;
  System__LStrArrayClr_51F89FF7(&local_18,2);
  puStack_40 = (undefined1 *)0x557a88aa;
  System__LStrClr_40929B27(&local_8);
  return;
}

