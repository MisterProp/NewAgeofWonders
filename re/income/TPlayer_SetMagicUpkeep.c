

void AoWE_TPlayer_SetMagicUpkeep_23EDC2EF(int param_1,undefined4 param_2)



{

                    /* 0x520ec  2619  AoWE.TPlayer.SetMagicUpkeep@23EDC2EF */

  *(undefined4 *)(param_1 + 0xcc) = param_2;

  if (*(int *)(param_1 + 8) == 0) {

    Engine_TEventList_TriggerNotifyEvents_23EDC2EF(*(undefined4 *)(param_1 + 0x14),param_1);

  }

  return;

}




