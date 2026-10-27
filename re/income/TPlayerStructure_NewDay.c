

void AoWE_TPlayerStructure_NewDay_23EDC2EF(int *param_1)



{

                    /* 0x60a90  2211  AoWE.TPlayerStructure.NewDay@23EDC2EF */

  AoWE_TStructure_NewDay_23EDC2EF(param_1);

  if ((*(int *)(AoWE_AoWHSMap_6B957A58 + 0x174) == 1) && ('\0' < (char)param_1[0xc])) {

    (**(code **)(*param_1 + 0x208))();

    AoWE_TPlayerStructure_UpdatePlayerSeen_23EDC2EF(param_1);

  }

  return;

}




