
undefined4 AoWE_TAoWHSMap_CancelEndTurn_23EDC2EF(int param_1)

{
  undefined4 uVar1;
  
                    /* 0x783c8  1395  AoWE.TAoWHSMap.CancelEndTurn@23EDC2EF */
  if (*(char *)(param_1 + 0xa5) != -1) {
    uVar1 = FUN_557475c8(*(char *)(param_1 + 0xa5));
    return uVar1;
  }
  return 0;
}

