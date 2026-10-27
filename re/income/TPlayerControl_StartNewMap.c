

void AoWE_TPlayerControl_StartNewMap_23EDC2EF(void)



{

  int iVar1;

  int iVar2;

  int iVar3;

  

                    /* 0x5573c  2545  AoWE.TPlayerControl.StartNewMap@23EDC2EF */

  iVar1 = (**(code **)(**(int **)(AoWE_AoWHSMap_6B957A58 + 0x140) + 0x54))();

  if (-1 < iVar1 + -1) {

    iVar3 = 0;

    do {

      if (iVar3 == 0) {

LAB_55755777:

        iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                          (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar3);

        *(undefined1 *)(iVar2 + 0x45) = 1;

      }

      else {

        iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                          (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar3);

        if (*(char *)(iVar2 + 0xa7) != '\x04') goto LAB_55755777;

      }

      iVar3 = iVar3 + 1;

      iVar1 = iVar1 + -1;

    } while (iVar1 != 0);

  }

  return;

}




