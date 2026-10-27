

void AoWE_TPlayer_RegisterIncomeSource_23EDC2EF(int param_1,undefined4 param_2)



{

  int iVar1;

  

                    /* 0x52068  2623  AoWE.TPlayer.RegisterIncomeSource@23EDC2EF */

  iVar1 = Classes_TList_IndexOf_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 100) + 8),param_2);

  if (iVar1 == -1) {

    Classes_TList_Add_23EDC2EF(*(undefined4 *)(*(int *)(param_1 + 100) + 8),param_2);

    if (*(int *)(param_1 + 8) == 0) {

      AoWE_TPlayer_UpdateIncome_23EDC2EF(param_1);

    }

  }

  return;

}




