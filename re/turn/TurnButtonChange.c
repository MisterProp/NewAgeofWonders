
void TurnButtonChange(int param_1)

{
  undefined1 uVar1;
  byte bVar2;
  int iVar3;
  
  iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  if (*(char *)(iVar3 + 0x2c) == '\0') {
    uVar1 = AoWE_TAoWHSMap_EndTurn_23EDC2EF(*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref);
    *(undefined1 *)(*(int *)(param_1 + 0x5c) + 0x13c) = uVar1;
    return;
  }
  bVar2 = AoWE_TAoWHSMap_CancelEndTurn_23EDC2EF(*(undefined4 *)AoWE_AoWHSMap_6B957A58_exref);
  *(byte *)(*(int *)(param_1 + 0x5c) + 0x13c) = bVar2 ^ 1;
  return;
}

