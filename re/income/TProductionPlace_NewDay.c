

void PrdPlace_TProductionPlace_NewDay_23EDC2EF(int *param_1)



{

                    /* 0xbf2f8  7443  PrdPlace.TProductionPlace.NewDay@23EDC2EF */

  AoWE_TPlayerStructure_NewDay_23EDC2EF(param_1);

  if (*(int *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x174) == 1) {

    if ((char)param_1[0xc] == '\0') {

      (**(code **)(*param_1 + 0x228))();

    }

    PrdPlace_TProductionPlace_ResetProductions_23EDC2EF(param_1);

  }

  return;

}




