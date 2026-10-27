

void AoWE_TPlayer_SetArmyUpkeep_23EDC2EF(int param_1,undefined4 param_2)



{

                    /* 0x520d4  2620  AoWE.TPlayer.SetArmyUpkeep@23EDC2EF */

  *(undefined4 *)(param_1 + 200) = param_2;

  if (*(int *)(param_1 + 8) == 0) {

    Engine_TEventList_TriggerNotifyEvents_23EDC2EF(*(undefined4 *)(param_1 + 0x14),param_1);

  }

  return;

}




