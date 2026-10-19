
int AoWE_TArmy_SelectionUnit_23EDC2EF(int param_1,uint param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
                    /* 0x8e8b4  364  AoWE.TArmy.SelectionUnit@23EDC2EF */
  iVar2 = 0;
  iVar1 = 0;
  while ((iVar1 != *(int *)(*(int *)(param_1 + 8) + 8) && (iVar2 == 0))) {
    if ((param_2 & 1) != 0) {
      if (param_3 == 0) {
        iVar2 = *(int *)(*(int *)(*(int *)(param_1 + 8) + 4) + iVar1 * 4);
      }
      param_3 = param_3 + -1;
    }
    param_2 = (param_2 & 0xff) >> 1;
    iVar1 = iVar1 + 1;
  }
  return iVar2;
}

