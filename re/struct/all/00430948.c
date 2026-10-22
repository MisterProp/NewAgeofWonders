
void FUN_00430948(int param_1)

{
  int iVar1;
  
  FUN_004302d8(param_1);
  (**(code **)(**(int **)(param_1 + 0x120) + 0x6c))(*(int **)(param_1 + 0x120),0);
  (**(code **)(**(int **)(param_1 + 0x118) + 0x6c))(*(int **)(param_1 + 0x118),0);
  (**(code **)(**(int **)(param_1 + 0x11c) + 0x6c))(*(int **)(param_1 + 0x11c),0);
  (**(code **)(**(int **)(param_1 + 0x124) + 0xa4))(*(int **)(param_1 + 0x124),0xe);
  (**(code **)(**(int **)(param_1 + 0x124) + 0xa8))(*(int **)(param_1 + 0x124),0xf);
  (**(code **)(**(int **)(param_1 + 0x124) + 0xa0))(*(int **)(param_1 + 0x124),0xd);
  (**(code **)(**(int **)(param_1 + 300) + 0x6c))(*(int **)(param_1 + 300),0);
  (**(code **)(**(int **)(param_1 + 0x128) + 0x6c))(*(int **)(param_1 + 0x128),0);
  iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  if (*(int *)(*(int *)(iVar1 + 0x54) + 0x34) == 0) {
    FUN_00430228(param_1,1);
  }
  else {
    FUN_00430228(param_1,0);
  }
  *(undefined1 *)(param_1 + 0x220) = 2;
  *(undefined1 *)(param_1 + 0x222) = 2;
  *(undefined4 *)(param_1 + 0x228) = 0;
  iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
  *(undefined4 *)(param_1 + 0x22c) = *(undefined4 *)(iVar1 + 0xd4);
  FUN_0042ef14(param_1);
  FUN_0042f668(param_1);
  FUN_0042e75c(param_1);
  return;
}

