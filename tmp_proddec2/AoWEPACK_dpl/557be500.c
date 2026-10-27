// /AoWEPACK.dpl : 557be500

int PrdPlace_TProductionPlace_GetIncome_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  
                    /* 0xbe500  7470  PrdPlace.TProductionPlace.GetIncome@23EDC2EF */
  iVar2 = (**(code **)(*param_1 + 0x22c))();
  cVar1 = (**(code **)(*param_1 + 0x230))();
  if (cVar1 == '\b') {
    if ((*(char *)(param_1[0x11] + 0x20) == '\x02') &&
       (iVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF(*(undefined4 *)(param_1[0x11] + 0x14),0),
       *(int *)(iVar3 + 8) != 10)) {
      return iVar2;
    }
    iVar3 = System__ROUND_51F89FF7();
    iVar2 = iVar2 + iVar3;
  }
  return iVar2;
}

