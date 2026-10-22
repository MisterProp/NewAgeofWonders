
void Crops_TPlayerCropStructure_CropTrampled_23EDC2EF(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  
                    /* 0xa6108  6742  Crops.TPlayerCropStructure.CropTrampled@23EDC2EF */
  if ((((char)param_2 != *(char *)(param_1 + 0x30)) && ('\0' < *(char *)(param_1 + 0x30))) &&
     ('\0' < (char)param_2)) {
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                       (int)*(char *)(param_1 + 0x30));
    cVar1 = *(char *)(iVar2 + 0xa5);
    uVar3 = AoWE_TRaceList_GetRace_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                       (int)cVar1);
    iVar2 = AoWE_TRace_GetPlayerRelationValue_23EDC2EF(uVar3,param_2);
    uVar3 = AoWE_TRaceList_GetRace_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x144),
                       (int)cVar1);
    AoWE_TRace_SetPlayerRelationValue_23EDC2EF(uVar3,param_2,iVar2 + 5);
  }
  return;
}

