
void FUN_00423650(int param_1,undefined4 param_2,char param_3)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  
  if ((-1 < *(int *)(*(int *)(param_1 + 0x54) + 0x120)) && (param_3 == '\x01')) {
    piVar3 = *(int **)(*(int *)(param_1 + 0x54) + 0x114);
    uVar1 = (**(code **)(*piVar3 + 0x18))(piVar3,*(undefined4 *)(*(int *)(param_1 + 0x54) + 0x120));
    iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    piVar3 = (int *)EventLog_TEventLogbook_GetLogs_23EDC2EF(*(undefined4 *)(iVar2 + 0xd8),uVar1);
    (**(code **)(*piVar3 + 0x74))();
  }
  return;
}

