// /AoWEPACK.dpl : 557a88bc

void City_TCityProductionControl_ExecuteCancelProduction_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
                    /* 0xa88bc  6960  City.TCityProductionControl.ExecuteCancelProduction@23EDC2EF
                        */
  if ((char)param_1[8] == '\x02') {
    iVar2 = AoWE_TProductionQueue_GetItem_23EDC2EF(param_1[5],0);
    cVar1 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(iVar2 + 8));
    if (cVar1 == '\x04') {
      if (*(char *)(param_1[9] + 0x10) == *(char *)(param_1[9] + 0x1c)) {
        uVar3 = (**(code **)(*param_1 + 0x7c))();
        uVar4 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                           (int)*(char *)(param_1[10] + 0x45));
        iVar2 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar4,uVar3);
        uVar3 = (**(code **)(*param_1 + 0x7c))();
        uVar4 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                           (int)*(char *)(param_1[10] + 0x45));
        AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar4,uVar3,iVar2 + 0xf);
      }
      else {
        uVar3 = (**(code **)(*param_1 + 0x7c))();
        uVar4 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                           (int)*(char *)(param_1[10] + 0x45));
        iVar2 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar4,uVar3);
        uVar3 = (**(code **)(*param_1 + 0x7c))();
        uVar4 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                           (int)*(char *)(param_1[10] + 0x45));
        AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar4,uVar3,iVar2 + 10);
      }
    }
    else if (cVar1 == '\x05') {
      if (*(char *)(param_1[9] + 0x10) == *(char *)(param_1[9] + 0x1c)) {
        uVar3 = (**(code **)(*param_1 + 0x7c))();
        uVar4 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                           (int)*(char *)(param_1[10] + 0x45));
        iVar2 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar4,uVar3);
        uVar3 = (**(code **)(*param_1 + 0x7c))();
        uVar4 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                           (int)*(char *)(param_1[10] + 0x45));
        AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar4,uVar3,iVar2 + 0x1e);
      }
      else {
        uVar3 = (**(code **)(*param_1 + 0x7c))();
        uVar4 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                           (int)*(char *)(param_1[10] + 0x45));
        iVar2 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar4,uVar3);
        uVar3 = (**(code **)(*param_1 + 0x7c))();
        uVar4 = AoWE_TRaceList_GetRace_23EDC2EF
                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                           (int)*(char *)(param_1[10] + 0x45));
        AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar4,uVar3,iVar2 + 0x14);
      }
    }
  }
  AoWE_TProductionControl_ExecuteCancelProduction_23EDC2EF(param_1);
  return;
}

