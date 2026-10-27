// /AoWEPACK.dpl : 55751f38


void AoWE_TPlayer_SetGems_23EDC2EF(int param_1,undefined4 param_2)



{

                    /* 0x51f38  2625  AoWE.TPlayer.SetGems@23EDC2EF */

  *(undefined4 *)(param_1 + 0xc4) = param_2;

  if (*(int *)(param_1 + 8) == 0) {

    Engine_TEventList_TriggerNotifyEvents_23EDC2EF(*(undefined4 *)(param_1 + 0xc),param_1);

  }

  if ((*(char *)(AoWE_AoWHSMap_6B957A58 + 0x14d) == '\x02') &&

     (*(int *)(AoWE_AoWHSMap_6B957A58 + 0x154) <= *(int *)(param_1 + 0xc4))) {

    AoWE_TAoWHSMap_GameOver_23EDC2EF(AoWE_AoWHSMap_6B957A58,*(undefined1 *)(param_1 + 0xa6),1);

  }

  return;

}



