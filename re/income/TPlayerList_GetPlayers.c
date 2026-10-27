

undefined4 AoWE_TPlayerList_GetPlayers_23EDC2EF(int param_1,int param_2)



{

                    /* 0x544d0  2572  AoWE.TPlayerList.GetPlayers@23EDC2EF */

  if ((-1 < param_2) && (param_2 < *(int *)(*(int *)(param_1 + 8) + 8))) {

    return *(undefined4 *)(*(int *)(*(int *)(param_1 + 8) + 4) + param_2 * 4);

  }

  return 0;

}




