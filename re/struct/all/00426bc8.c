
void FUN_00426bc8(int param_1,undefined4 param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 local_5c [15];
  undefined1 local_20 [16];
  
  puVar5 = local_5c;
  for (iVar4 = 0xf; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar5 = *param_3;
    param_3 = param_3 + 1;
    puVar5 = puVar5 + 1;
  }
  iVar4 = *(int *)(*(int *)(param_1 + 0x140) + 0x88);
  iVar1 = *(int *)(*(int *)(param_1 + 0x140) + 0x84);
  Classes_Rect_9923E97E
            (iVar1 + 1,iVar4,iVar1 + *(int *)(*(int *)(param_1 + 0x140) + 0x7c),local_20,
             iVar4 + *(int *)(*(int *)(param_1 + 0x140) + 0x80));
  puVar5 = local_5c;
  if (*(int *)(param_1 + 0x27c) == 0) {
    if (*(char *)(param_1 + 0x286) == -1) {
      AoWE_TRaceResourceList_GetRaceResource_23EDC2EF
                (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x54),
                 (int)*(char *)(param_1 + 0x287));
      piVar3 = (int *)AoWE_TRaceResource_GetFace_23EDC2EF();
      (**(code **)(*piVar3 + 0x84))
                (piVar3,*(int *)(*(int *)(param_1 + 0x140) + 0x84) + 1,
                 *(int *)(*(int *)(param_1 + 0x140) + 0x88) + 3,puVar5);
    }
    else {
      uVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      iVar4 = AoWE_TPlayer_GetFirstDaySeen_23EDC2EF(uVar2,*(undefined1 *)(param_1 + 0x286));
      if (iVar4 != -1) {
        iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)*(char *)(param_1 + 0x286));
        AoWE_TAbstractUnit_ShowFace_23EDC2EF(*(undefined4 *)(iVar4 + 0xd4),local_20,local_5c);
      }
    }
  }
  else {
    iVar4 = *(int *)(param_1 + 0x29c);
    if ((iVar4 == 0) || (*(char *)(*(int *)(iVar4 + 0x18) + 0x13) != '\a')) {
      iVar4 = *(int *)(param_1 + 0x27c);
      if (*(char *)(iVar4 + 0x10) == *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5)) {
        iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)*(char *)(iVar4 + 0x11));
        AoWE_TAbstractUnit_ShowFace_23EDC2EF(*(undefined4 *)(iVar4 + 0xd4),local_20,local_5c);
      }
      else if (*(char *)(iVar4 + 0x14) == '\0') {
        iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)*(char *)(*(int *)(param_1 + 0x27c) + 0x10));
        AoWE_TAbstractUnit_ShowFace_23EDC2EF(*(undefined4 *)(iVar4 + 0xd4),local_20,local_5c);
      }
      else {
        iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                           (int)*(char *)(iVar4 + 0x11));
        AoWE_TAbstractUnit_ShowFace_23EDC2EF(*(undefined4 *)(iVar4 + 0xd4),local_20,local_5c);
      }
    }
    else {
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)(iVar4 + 0x18) + 0x12));
      AoWE_TAbstractUnit_ShowFace_23EDC2EF(*(undefined4 *)(iVar4 + 0xd4),local_20,local_5c);
    }
  }
  return;
}

