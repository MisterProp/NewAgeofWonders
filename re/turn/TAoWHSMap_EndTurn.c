
undefined4 AoWE_TAoWHSMap_EndTurn_23EDC2EF(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
                    /* 0x7838c  1396  AoWE.TAoWHSMap.EndTurn@23EDC2EF */
  if (*(char *)(param_1 + 0xa5) == -1) {
    uVar2 = 0;
  }
  else {
    if (*(char *)(param_1 + 0x139) != '\0') {
      iVar1 = EventLog_TExecuteEventLogCallBackList_GetCount_23EDC2EF
                        (*(undefined4 *)(param_1 + 0x1d4));
      if (iVar1 != 0) {
        return 0;
      }
    }
    uVar2 = FUN_5574757c(*(undefined1 *)(param_1 + 0xa5));
  }
  return uVar2;
}

