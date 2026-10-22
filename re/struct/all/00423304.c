
void FUN_00423304(int param_1)

{
  int iVar1;
  int iVar2;
  
  (**(code **)(**(int **)(*(int *)(param_1 + 0x54) + 0x114) + 0x40))();
  if (-1 < *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) {
    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    EventLog_TEventLogbook_ListEvents_23EDC2EF
              (*(undefined4 *)(iVar1 + 0xd8),*(undefined4 *)(*(int *)(param_1 + 0x54) + 0x114));
  }
  iVar1 = *(int *)(param_1 + 0x54);
  iVar2 = (**(code **)(**(int **)(iVar1 + 0x114) + 0x14))();
  AoWListBox_TAOWListBox_SetListOff_23EDC2EF(iVar1,iVar2 + -1);
  return;
}

