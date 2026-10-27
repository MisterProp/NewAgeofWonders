// /AoWEPACK.dpl : 5575c62c

int AoWE_TUnitProductionList_IndexOf_23EDC2EF(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
                    /* 0x5c62c  2393  AoWE.TUnitProductionList.IndexOf@23EDC2EF */
  iVar1 = (**(code **)(*param_1 + 0x54))();
  do {
    if (iVar1 == 0) {
      return -1;
    }
    iVar1 = iVar1 + -1;
    iVar2 = AoWE_TUnitProductionList_GetItem_23EDC2EF(param_1,iVar1);
  } while (param_2 != *(int *)(iVar2 + 8));
  return iVar1;
}

