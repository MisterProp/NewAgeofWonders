
void FUN_00457508(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = 0;
  iVar1 = (**(code **)(**(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140) + 0x54))();
  if (-1 < iVar1 + -1) {
    iVar4 = 0;
    do {
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar4);
      if (*(char *)(iVar2 + 0x45) == '\x01') {
        iVar3 = iVar3 + 1;
      }
      iVar4 = iVar4 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  (**(code **)(**(int **)(param_1 + 0x94) + 0x60))(*(int **)(param_1 + 0x94),iVar3 * 5 + 4);
  iVar1 = *(int *)(param_1 + 0x94);
  *(undefined1 *)(iVar1 + 0x2d) = 0;
  iVar3 = *(int *)(param_1 + 0x50);
  *(int *)(*(int *)(iVar3 + 0xd4) + 0x1c) = *(int *)(iVar1 + 0x7c) + 0x116;
  *(undefined1 *)(iVar3 + 0x2d) = 0;
  return;
}

