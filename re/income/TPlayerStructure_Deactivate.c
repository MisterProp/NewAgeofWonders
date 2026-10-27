

void AoWE_TPlayerStructure_Deactivate_23EDC2EF(int *param_1)



{

  int iVar1;

  

                    /* 0x613e4  2192  AoWE.TPlayerStructure.Deactivate@23EDC2EF */

  if ((char)param_1[0xc] != -1) {

    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)(char)param_1[0xc]);

    (**(code **)(**(int **)(iVar1 + 0x1c) + 0x6c))(*(int **)(iVar1 + 0x1c),param_1);

  }

  AoWE_TPlayerStructure_UnregisterVictoryLocations_23EDC2EF(param_1);

  AoWE_TStructure_Deactivate_23EDC2EF(param_1);

  (**(code **)(*param_1 + 0x204))();

  return;

}




