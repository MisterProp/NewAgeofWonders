

void AoWE_TAoWHSMap_StartNewMap_23EDC2EF(int param_1)



{

  int iVar1;

  int iVar2;

  undefined4 uVar3;

  undefined4 uVar4;

  int iVar5;

  int iVar6;

  

                    /* 0x77d68  1403  AoWE.TAoWHSMap.StartNewMap@23EDC2EF */

  Engine_TByteList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x170));

  if ((*(char *)(param_1 + 0x16c) == '\0') &&

     (iVar1 = (**(code **)(**(int **)(param_1 + 0x140) + 0x54))(), -1 < iVar1 + -1)) {

    iVar6 = 0;

    do {

      Engine_TByteList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x170),iVar6);

      iVar6 = iVar6 + 1;

      iVar1 = iVar1 + -1;

    } while (iVar1 != 0);

  }

  if (*(char *)(param_1 + 0x16c) == '\x01') {

    AoWE_TAoWHSMap_GenerateNewTurnOrder_23EDC2EF(param_1);

  }

  iVar1 = (**(code **)(**(int **)(param_1 + 0x140) + 0x54))();

  if (-1 < iVar1 + -1) {

    iVar6 = 0;

    do {

      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

      uVar4 = *(undefined4 *)(iVar2 + 0xac);

      uVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

      AoWE_TPlayer_SetGems_23EDC2EF(uVar3,uVar4);

      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

      uVar4 = *(undefined4 *)(*(int *)(iVar2 + 0x54) + 0x18);

      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

      AoWE_TPlayerMagicControl_SetMana_23EDC2EF(*(undefined4 *)(iVar2 + 0x54),uVar4);

      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

      if (*(char *)(iVar2 + 0xa7) == '\x06') {

        iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

        iVar2 = *(int *)(iVar2 + 0xc4);

        uVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

        AoWE_TPlayer_SetGems_23EDC2EF(uVar4,iVar2 + 0xfa);

        iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

        iVar2 = *(int *)(*(int *)(iVar2 + 0x54) + 0x24);

        iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

        AoWE_TPlayerMagicControl_SetMana_23EDC2EF(*(undefined4 *)(iVar5 + 0x54),iVar2 + 0xfa);

      }

      else if (*(char *)(iVar2 + 0xa7) == '\a') {

        iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

        iVar2 = *(int *)(iVar2 + 0xc4);

        uVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

        AoWE_TPlayer_SetGems_23EDC2EF(uVar4,iVar2 + 500);

        iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

        iVar2 = *(int *)(*(int *)(iVar2 + 0x54) + 0x24);

        iVar5 = AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(param_1 + 0x140),iVar6);

        AoWE_TPlayerMagicControl_SetMana_23EDC2EF(*(undefined4 *)(iVar5 + 0x54),iVar2 + 500);

      }

      iVar6 = iVar6 + 1;

      iVar1 = iVar1 + -1;

    } while (iVar1 != 0);

  }

  (**(code **)(**(int **)(param_1 + 0x244) + 100))();

  return;

}




