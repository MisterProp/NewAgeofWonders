// /AoWEPACK.dpl : 5575cc90

undefined4
AoWE_TProductionControl_CancelProductionReturnGold_23EDC2EF
          (int param_1,undefined4 param_2,undefined4 param_3)

{
  char cVar1;
  int iVar2;
  longlong lVar3;
  undefined4 uVar4;
  
                    /* 0x5cc90  2368  AoWE.TProductionControl.CancelProductionReturnGold@23EDC2EF */
  if (*(char *)(param_1 + 0x20) == '\x02') {
    iVar2 = *(int *)(param_1 + 0x24);
    cVar1 = *(char *)(iVar2 + 0x1c);
    if (cVar1 == '\0') {
      uVar4 = 0;
    }
    else {
      lVar3 = (longlong)(int)*(char *)(iVar2 + 0x10) * (longlong)*(int *)(iVar2 + 8);
      uVar4 = System__ROUND_51F89FF7
                        ((int)cVar1,(int)((ulonglong)lVar3 >> 0x20),param_3,(int)lVar3,(int)cVar1);
    }
  }
  else {
    uVar4 = 0;
  }
  return uVar4;
}

