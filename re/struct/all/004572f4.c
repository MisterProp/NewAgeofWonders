
void TurnButtonAnimDone(int param_1)

{
  *(undefined1 *)(*(int *)(param_1 + 0x5c) + 0x13c) = 0;
  if (*(int *)AoWTC_AoWCombatMap_2AEF5FAB_exref != 0) {
    *(undefined1 *)(*(int *)AoWTC_AoWCombatMap_2AEF5FAB_exref + 0x128) = 1;
  }
  return;
}

