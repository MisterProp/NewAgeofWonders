

void AoWE_TRace_NewDay_23EDC2EF(int param_1)



{

  int iVar1;

  int iVar2;

  int iVar3;

  int iVar4;

  

                    /* 0x4ce60  2865  AoWE.TRace.NewDay@23EDC2EF */

  if (*(int *)(AoWE_AoWHSMap_6B957A58 + 0x174) == 1) {

    AoWE_TRace_ResetPlayerRelations_23EDC2EF(param_1);

  }

  else if (((*(byte *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x188) + 0x10) & 6) == 0) &&

          (iVar3 = *(int *)(*(int *)(param_1 + 0x10) + 8), -1 < iVar3 + -1)) {

    iVar4 = 0;

    do {

      iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),iVar4 + 1);

      Engine_TByteList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0xc),(int)*(char *)(iVar1 + 0xa5));

      iVar1 = AoWE_StatusToStatusValue_496002BF();

      iVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4);

      if (iVar1 < iVar2) {

        iVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4);

        Engine_TIntegerList_Put_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4,iVar2 + -1);

        iVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4);

        if (iVar2 < iVar1) {

          Engine_TIntegerList_Put_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4,iVar1);

        }

      }

      iVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4);

      if (iVar2 < iVar1) {

        iVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4);

        Engine_TIntegerList_Put_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4,iVar2 + 1);

        iVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4);

        if (iVar1 < iVar2) {

          Engine_TIntegerList_Put_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4,iVar1);

        }

      }

      iVar4 = iVar4 + 1;

      iVar3 = iVar3 + -1;

    } while (iVar3 != 0);

  }

  return;

}




