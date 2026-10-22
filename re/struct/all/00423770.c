
void FUN_00423770(int param_1,undefined4 param_2,undefined4 *param_3)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 local_5c [15];
  undefined1 local_20 [16];
  
  puVar4 = local_5c;
  for (iVar3 = 0xf; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *param_3;
    param_3 = param_3 + 1;
    puVar4 = puVar4 + 1;
  }
  if (-1 < *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) {
    iVar3 = *(int *)(*(int *)(param_1 + 0x74) + 0x88);
    iVar1 = *(int *)(*(int *)(param_1 + 0x74) + 0x84);
    Classes_Rect_9923E97E
              (iVar1 + 3,iVar3 + 3,iVar1 + *(int *)(*(int *)(param_1 + 0x74) + 0x7c) + -3,local_20,
               iVar3 + *(int *)(*(int *)(param_1 + 0x74) + 0x80) + -3);
    AoWE_TPlayerList_GetPlayers_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
               (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    iVar3 = AoWE_TPlayer_GetFace_23EDC2EF();
    if (iVar3 != 0) {
      AoWE_TPlayerList_GetPlayers_23EDC2EF
                (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                 (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      piVar2 = (int *)AoWE_TPlayer_GetFace_23EDC2EF();
      (**(code **)(*piVar2 + 0x88))(piVar2,local_20,local_5c);
    }
  }
  return;
}

