

void AoWE_TPlayer_UpdateIncome_23EDC2EF(int param_1)



{

  char cVar1;

  int iVar2;

  int iVar3;

  undefined4 uVar4;

  

                    /* 0x51f88  2624  AoWE.TPlayer.UpdateIncome@23EDC2EF */

  *(undefined4 *)(param_1 + 0xd0) = *(undefined4 *)(param_1 + 0xb0);

  iVar2 = (**(code **)(**(int **)(param_1 + 100) + 0x54))();

  iVar2 = iVar2 + -1;

  if (-1 < iVar2) {

    do {

      iVar3 = (**(code **)(**(int **)(*(int *)(*(int *)(*(int *)(param_1 + 100) + 8) + 4) +

                                     iVar2 * 4) + 0x50))();

      *(int *)(param_1 + 0xd0) = *(int *)(param_1 + 0xd0) + iVar3;

      iVar2 = iVar2 + -1;

    } while (iVar2 != -1);

  }

  cVar1 = *(char *)(param_1 + 0xa7);

  if (cVar1 == '\x02') {

    uVar4 = System__ROUND_51F89FF7();

    *(undefined4 *)(param_1 + 0xd0) = uVar4;

  }

  else if (cVar1 == '\x03') {

    uVar4 = System__ROUND_51F89FF7();

    *(undefined4 *)(param_1 + 0xd0) = uVar4;

  }

  else if (cVar1 == '\x06') {

    iVar2 = System__ROUND_51F89FF7();

    *(int *)(param_1 + 0xd0) = iVar2 + 0x32;

  }

  else if (cVar1 == '\a') {

    iVar2 = System__ROUND_51F89FF7();

    *(int *)(param_1 + 0xd0) = iVar2 + 100;

  }

  if (*(int *)(param_1 + 8) == 0) {

    Engine_TEventList_TriggerNotifyEvents_23EDC2EF(*(undefined4 *)(param_1 + 0x10),param_1);

  }

  return;

}




