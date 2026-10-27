

void Mine_TMine_SetPlayer_23EDC2EF(int param_1,undefined4 param_2)



{

  undefined4 uVar1;

  

                    /* 0xb4564  7170  Mine.TMine.SetPlayer@23EDC2EF */

  if ((char)param_2 != *(char *)(param_1 + 0x30)) {

    if ((*(byte *)(param_1 + 0xc) & 0x20) == 0) {

      AoWE_TPlayerStructure_SetPlayer_23EDC2EF(param_1,param_2);

    }

    else {

      if (*(char *)(param_1 + 0x30) != -1) {

        uVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                           (int)*(char *)(param_1 + 0x30));

        AoWE_TPlayer_UnregisterIncomeSource_23EDC2EF(uVar1,*(undefined4 *)(param_1 + 0x40));

      }

      AoWE_TPlayerStructure_SetPlayer_23EDC2EF(param_1,param_2);

      if (*(char *)(param_1 + 0x30) != -1) {

        uVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                          (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                           (int)*(char *)(param_1 + 0x30));

        AoWE_TPlayer_RegisterIncomeSource_23EDC2EF(uVar1,*(undefined4 *)(param_1 + 0x40));

      }

    }

  }

  return;

}




