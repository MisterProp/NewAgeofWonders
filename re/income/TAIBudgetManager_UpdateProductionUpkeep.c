

void AoWE_TAIBudgetManager_UpdateProductionUpkeep_23EDC2EF(int param_1)



{

  int iVar1;

  int iVar2;

  int iVar3;

  int iVar4;

  

                    /* 0x362a4  3522  AoWE.TAIBudgetManager.UpdateProductionUpkeep@23EDC2EF */

  iVar1 = *(int *)(param_1 + 8);

  *(undefined4 *)(param_1 + 0x24) = 0;

  *(undefined4 *)(param_1 + 0x20) = 0;

  iVar4 = *(int *)(*(int *)(iVar1 + 0x24) + 8) + -1;

  if (-1 < iVar4) {

    do {

      iVar2 = AoWE_TProductionControlList_GetPC_23EDC2EF(*(undefined4 *)(iVar1 + 0x24),iVar4);

      if (*(char *)(iVar2 + 0x20) == '\x02') {

        iVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(iVar2 + 0x14),0);

        if ((*(int *)(iVar3 + 8) == 1) && (*(int *)(*(int *)(iVar2 + 0x24) + 0x24) != 0)) {

          *(int *)(param_1 + 0x20) =

               *(int *)(param_1 + 0x20) + *(int *)(*(int *)(iVar2 + 0x24) + 0x14);

          *(int *)(param_1 + 0x24) =

               *(int *)(param_1 + 0x24) + *(int *)(*(int *)(iVar2 + 0x24) + 0x18);

        }

      }

      iVar4 = iVar4 + -1;

    } while (iVar4 != -1);

  }

  return;

}




