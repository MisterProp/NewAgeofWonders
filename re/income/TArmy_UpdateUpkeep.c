

void AoWE_TArmy_UpdateUpkeep_23EDC2EF(int *param_1)



{

  byte bVar1;

  char cVar2;

  int iVar3;

  undefined4 uVar4;

  byte bVar5;

  int iVar6;

  

                    /* 0x8cc08  419  AoWE.TArmy.UpdateUpkeep@23EDC2EF */

  bVar5 = 0;

  iVar3 = (**(code **)(*param_1 + 0x54))();

  if (-1 < iVar3 + -1) {

    iVar6 = 0;

    do {

      cVar2 = (**(code **)(**(int **)(*(int *)(param_1[2] + 4) + iVar6 * 4) + 0x9c))();

      bVar5 = bVar5 + cVar2;

      iVar6 = iVar6 + 1;

      iVar3 = iVar3 + -1;

    } while (iVar3 != 0);

  }

  bVar1 = *(byte *)(param_1 + 6);

  if ((bVar5 != bVar1) && ((*(byte *)(param_1 + 4) & 1) != 0)) {

    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),

                       (int)*(char *)((int)param_1 + 0x12));

    iVar3 = *(int *)(iVar3 + 200);

    uVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                      (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),

                       (int)*(char *)((int)param_1 + 0x12));

    AoWE_TPlayer_SetArmyUpkeep_23EDC2EF(uVar4,((uint)bVar5 - (uint)bVar1) + iVar3);

  }

  *(byte *)(param_1 + 6) = bVar5;

  return;

}




