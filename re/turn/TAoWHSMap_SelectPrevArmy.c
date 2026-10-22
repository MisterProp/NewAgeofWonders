
undefined4 AoWE_TAoWHSMap_SelectPrevArmy_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int *local_1c;
  undefined4 auStack_18 [2];
  
                    /* 0x76118  1459  AoWE.TAoWHSMap.SelectPrevArmy@23EDC2EF */
  local_1c = param_1;
  iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF(param_1[0x50],(int)*(char *)((int)param_1 + 0xa5));
  iVar3 = (**(code **)(**(int **)(iVar2 + 0x28) + 0x54))();
  if (iVar3 == 0) {
    return 1;
  }
  iVar3 = *(int *)(iVar2 + 0xb4);
  if ((iVar3 < 0) || (iVar4 = (**(code **)(**(int **)(iVar2 + 0x28) + 0x54))(), iVar4 <= iVar3)) {
    iVar3 = 0;
  }
  iVar4 = (**(code **)(**(int **)(iVar2 + 0x28) + 0x54))();
  do {
    iVar3 = iVar3 + -1;
    iVar4 = iVar4 + -1;
    if (iVar3 < 0) {
      iVar3 = (**(code **)(**(int **)(iVar2 + 0x28) + 0x54))();
      iVar3 = iVar3 + -1;
    }
    if (iVar4 == 0) break;
    uVar5 = AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar2 + 0x28),iVar3);
    cVar1 = AoWE_TArmy_MovePoints_23EDC2EF(uVar5,0xff);
  } while ((cVar1 < '\x02') ||
          (iVar6 = AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar2 + 0x28),iVar3),
          (*(byte *)(iVar6 + 0x28) & 3) != 0));
  uVar5 = AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar2 + 0x28),iVar3);
  cVar1 = AoWE_TArmy_MovePoints_23EDC2EF(uVar5,0xff);
  if (('\x01' < cVar1) &&
     (iVar4 = AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar2 + 0x28),iVar3),
     (*(byte *)(iVar4 + 0x28) & 3) == 0)) {
    iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (local_1c[0x50],(int)*(char *)((int)local_1c + 0xa5));
    iVar4 = AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar4 + 0x28),iVar3);
    cVar1 = *(char *)(iVar4 + 0x15);
    (**(code **)(*local_1c + 0xbc))
              (local_1c,(int)*(char *)(iVar4 + 0x13),(int)*(char *)(iVar4 + 0x14));
    HSEngine_THexagonSpriteSelection_UnselectLocation_23EDC2EF(*(undefined4 *)(cVar1 + 0x18));
    HSEngine_HXtoHP_8E9B8701
              ((int)*(char *)(iVar4 + 0x13),(int)*(char *)(iVar4 + 0x14),&local_1c,auStack_18);
    HSEngine_THexagonSpriteSelection_SelectLocation_23EDC2EF
              (*(undefined4 *)(cVar1 + 0x18),local_1c,auStack_18[0],(int)*(char *)(iVar4 + 0x15));
    *(int *)(iVar2 + 0xb4) = iVar3;
    return 1;
  }
  HSEngine_THexagonSpriteSelection_UnselectLocation_23EDC2EF(local_1c[6]);
  return 1;
}

