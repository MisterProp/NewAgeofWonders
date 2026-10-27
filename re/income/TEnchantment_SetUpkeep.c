

void AoWE_TEnchantment_SetUpkeep_23EDC2EF(int param_1,int param_2)



{

  int iVar1;

  

                    /* 0x7c10c  1233  AoWE.TEnchantment.SetUpkeep@23EDC2EF */

  if (param_2 != *(int *)(param_1 + 0x10)) {

    *(int *)(param_1 + 0x10) = param_2;

    if (-1 < *(char *)(param_1 + 0xc)) {

      iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),

                         (int)*(char *)(param_1 + 0xc));

      AoWE_TPlayerMagicControl_Update_23EDC2EF(*(undefined4 *)(iVar1 + 0x54));

    }

  }

  return;

}




