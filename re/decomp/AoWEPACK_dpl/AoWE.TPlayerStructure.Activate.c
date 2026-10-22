
void AoWE_TPlayerStructure_Activate_23EDC2EF(int *param_1)

{
  int iVar1;
  
                    /* 0x61394  2193  AoWE.TPlayerStructure.Activate@23EDC2EF */
  AoWE_TStructure_Activate_23EDC2EF(param_1);
  (**(code **)(*param_1 + 0x1fc))();
  (**(code **)(*param_1 + 0x204))();
  AoWE_TPlayerStructure_RegisterVictoryLocations_23EDC2EF(param_1);
  if ((char)param_1[0xc] != -1) {
    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)(char)param_1[0xc]);
    (**(code **)(**(int **)(iVar1 + 0x1c) + 0x70))(*(int **)(iVar1 + 0x1c),param_1);
  }
  return;
}

