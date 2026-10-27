

void AoWE_TPlayer_SetEndTurn_23EDC2EF(int param_1,char param_2)



{

                    /* 0x52d00  2603  AoWE.TPlayer.SetEndTurn@23EDC2EF */

  if (param_2 != *(char *)(param_1 + 0x2c)) {

    *(char *)(param_1 + 0x2c) = param_2;

    AoWE_TAoWHSMap_TriggerEndTurnEvent_23EDC2EF

              (AoWE_AoWHSMap_6B957A58,*(undefined1 *)(param_1 + 0xa6));

  }

  return;

}




