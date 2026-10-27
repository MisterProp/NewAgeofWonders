

int AoWE_TPlayerMagicControl_GetPower_23EDC2EF(int param_1)



{

  int iVar1;

  int *piVar2;

  int iVar3;

  int iVar4;

  

                    /* 0x7ca00  1202  AoWE.TPlayerMagicControl.GetPower@23EDC2EF */

  iVar4 = 0;

  iVar1 = (**(code **)(**(int **)(param_1 + 0x40) + 0x54))();

  iVar1 = iVar1 + -1;

  if (-1 < iVar1) {

    do {

      piVar2 = (int *)AoWE_TPowerSourceList_GetItems_23EDC2EF(*(undefined4 *)(param_1 + 0x40),iVar1)

      ;

      iVar3 = (**(code **)(*piVar2 + 0x50))();

      iVar4 = iVar4 + iVar3;

      iVar1 = iVar1 + -1;

    } while (iVar1 != -1);

  }

  return iVar4;

}




