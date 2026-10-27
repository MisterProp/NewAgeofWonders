// /AoWEPACK.dpl : 557a97d8

void City_TCity_BuildingDone_23EDC2EF(int *param_1)

{
  int iVar1;
  
                    /* 0xa97d8  6935  City.TCity.BuildingDone@23EDC2EF */
  *(undefined1 *)((int)param_1 + 0x4d) = 1;
  *(undefined1 *)(param_1 + 0x13) = 0;
  iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                     (int)(char)param_1[9]);
  (**(code **)(*param_1 + 0x244))(param_1,*(undefined1 *)(iVar1 + 0xa5));
  AoWE_TPlayerStructure_BuildingDone_23EDC2EF(param_1);
  return;
}

