

void Crops_TPlayerCropStructure_SetIncome_23EDC2EF(int param_1,char param_2)



{

                    /* 0xa617c  6741  Crops.TPlayerCropStructure.SetIncome@23EDC2EF */

  if ((param_2 != *(char *)(param_1 + 0x3c)) && ((*(byte *)(param_1 + 0xc) & 0x20) != 0)) {

    *(char *)(param_1 + 0x3c) = param_2;

    if (*(char *)(param_1 + 0x30) != -1) {

      AoWE_TPlayerList_GetPlayers_23EDC2EF

                (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                 (int)*(char *)(param_1 + 0x30));

      AoWE_TPlayer_UpdateIncome_23EDC2EF();

    }

  }

  return;

}




