
undefined4 FUN_5574757c(undefined4 param_1)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  char cVar4;
  
  cVar4 = (char)param_1;
  if (cVar4 != -1) {
    AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar4)
    ;
    cVar1 = AoWE_TPlayer_GetBusy_23EDC2EF();
    if (cVar1 == '\0') {
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar4);
      if (*(char *)(iVar2 + 0x2c) == '\0') {
        uVar3 = FUN_5574752c(param_1);
        return uVar3;
      }
    }
  }
  return 0;
}

