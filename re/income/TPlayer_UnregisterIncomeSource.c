

void AoWE_TPlayer_UnregisterIncomeSource_23EDC2EF(int param_1,undefined4 param_2)



{

  int iVar1;

  

                    /* 0x5209c  2622  AoWE.TPlayer.UnregisterIncomeSource@23EDC2EF */

  iVar1 = Classes_TList_Remove_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 100) + 8),param_2);

  if ((iVar1 != -1) && (*(int *)(param_1 + 8) == 0)) {

    AoWE_TPlayer_UpdateIncome_23EDC2EF(param_1);

  }

  return;

}




