

char City_TCity_ExecuteRaze_23EDC2EF(int param_1,undefined4 param_2,undefined4 param_3)



{

  char cVar1;

  undefined4 uVar2;

  int iVar3;

  

                    /* 0xab414  6879  City.TCity.ExecuteRaze@23EDC2EF */

  cVar1 = AoWE_TStructure_ExecuteRaze_23EDC2EF(param_1,param_2,param_3,param_3);

  if (cVar1 != '\0') {

    uVar2 = AoWE_TRaceList_GetRace_23EDC2EF

                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),

                       (int)*(char *)(param_1 + 0x45));

    iVar3 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar2,param_2);

    uVar2 = AoWE_TRaceList_GetRace_23EDC2EF

                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),

                       (int)*(char *)(param_1 + 0x45));

    AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar2,param_2,iVar3 + -0x1e);

  }

  return cVar1;

}




