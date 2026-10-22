
void FUN_00457584(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  iVar1 = AoWTC_TTacticalCombatPlayerList_GetCount_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWTC_AoWCombatMap_2AEF5FAB_exref + 0xe0));
  if (-1 < iVar1 + -1) {
    do {
      iVar2 = iVar2 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  (**(code **)(**(int **)(param_1 + 200) + 0x60))(*(int **)(param_1 + 200),iVar2 * 5 + 4);
  iVar1 = *(int *)(param_1 + 200);
  *(undefined1 *)(iVar1 + 0x2d) = 0;
  iVar2 = *(int *)(param_1 + 0xa8);
  *(int *)(*(int *)(iVar2 + 0xd4) + 0x1c) = *(int *)(iVar1 + 0x7c) + 0x116;
  *(undefined1 *)(iVar2 + 0x2d) = 0;
  return;
}

