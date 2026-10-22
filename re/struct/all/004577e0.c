
void FUN_004577e0(int param_1)

{
  undefined1 uVar1;
  
  if (*(char *)(*(int *)(param_1 + 0xac) + 0x13c) != '\0') {
    uVar1 = AoWTC_TAoWCombatMap_EndTurn_23EDC2EF(*(undefined4 *)AoWTC_AoWCombatMap_2AEF5FAB_exref,0)
    ;
    *(undefined1 *)(*(int *)(param_1 + 0xac) + 0x13c) = uVar1;
    return;
  }
  AoWTC_TAoWCombatMap_CancelEndTurn_23EDC2EF(*(undefined4 *)AoWTC_AoWCombatMap_2AEF5FAB_exref);
  return;
}

