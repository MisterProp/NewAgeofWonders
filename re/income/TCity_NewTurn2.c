

void City_TCity_NewTurn_23EDC2EF(int param_1,undefined4 param_2)



{

  int iVar1;

  

                    /* 0xab814  6873  City.TCity.NewTurn@23EDC2EF */

  Crops_TPlayerCropStructure_NewTurn_23EDC2EF(param_1,param_2);

  if ((*(byte *)(param_1 + 0x44) & 1) == 0) {

    (**(code **)(**(int **)(param_1 + 0x50) + 0xa8))(*(int **)(param_1 + 0x50),param_2);

  }

  if ((char)param_2 == *(char *)(param_1 + 0x30)) {

    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                       (int)(char)param_2);

    if (*(char *)(iVar1 + 0xa7) != '\0') {

      City_TCity_UpdateAIMigration_23EDC2EF(param_1);

    }

  }

  return;

}




