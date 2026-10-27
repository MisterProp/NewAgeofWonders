

void AoWE_TPlayerList_Update_23EDC2EF(int param_1)



{

  int iVar1;

  char cVar2;

  int iVar3;

  int iVar4;

  

                    /* 0x54370  2576  AoWE.TPlayerList.Update@23EDC2EF */

  if (*(int *)(*(int *)(AoWE_AoWHSMap_6B957A58 + 0x240) + 0x1c) != 0) {

    Engine_TByteList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x14));

    Engine_TByteList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x1c));

    Engine_TByteList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x18));

    Engine_TByteList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x10));

    iVar3 = *(int *)(*(int *)(param_1 + 8) + 8);

    if (-1 < iVar3 + -1) {

      iVar4 = 0;

      do {

        iVar1 = *(int *)(*(int *)(*(int *)(param_1 + 8) + 4) + iVar4 * 4);

        if (*(char *)(iVar1 + 0x45) == '\x01') {

          if ((*(char *)(iVar1 + 0x44) != '\x02') && (iVar4 != 0)) {

            Engine_TByteList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x10),iVar4);

          }

          cVar2 = AoWE_TPlayer_GetLocal_23EDC2EF(iVar1);

          if (cVar2 != '\0') {

            Engine_TByteList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x14),iVar4);

            if (*(char *)(iVar1 + 0xa7) == '\0') {

              Engine_TByteList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),iVar4);

            }

          }

          if (*(char *)(iVar1 + 0xa7) == '\0') {

            Engine_TByteList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x18),iVar4);

          }

        }

        iVar4 = iVar4 + 1;

        iVar3 = iVar3 + -1;

      } while (iVar3 != 0);

    }

  }

  return;

}




