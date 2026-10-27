

int AoWE_TAIBudgetManager_GetBudgetGoldUpkeep_23EDC2EF(int param_1)



{

  int iVar1;

  int iVar2;

  int iVar3;

  

                    /* 0x36158  3529  AoWE.TAIBudgetManager.GetBudgetGoldUpkeep@23EDC2EF */

  iVar3 = 0;

  iVar1 = (**(code **)(**(int **)(param_1 + 0xc) + 0x54))();

  iVar1 = iVar1 + -1;

  if (-1 < iVar1) {

    do {

      iVar2 = AoWE_TAIBudgetList_GetBudgets_23EDC2EF(*(undefined4 *)(param_1 + 0xc),iVar1);

      iVar3 = iVar3 + *(int *)(iVar2 + 0x14);

      iVar1 = iVar1 + -1;

    } while (iVar1 != -1);

  }

  return iVar3;

}




