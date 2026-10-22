
void FUN_0044b3e8(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  
  iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                    (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                     (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5),param_3,param_3);
  iVar1 = (**(code **)(**(int **)(iVar1 + 0x28) + 0x54))();
  if (-1 < iVar1 + -1) {
    iVar7 = 0;
    do {
      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                         (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
      piVar3 = (int *)AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar2 + 0x28),iVar7);
      iVar2 = (**(code **)(*piVar3 + 0x54))();
      if (-1 < iVar2 + -1) {
        iVar6 = 0;
        do {
          iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                            (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                             (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
          uVar5 = AoWE_TArmyList_GetArmy_23EDC2EF(*(undefined4 *)(iVar4 + 0x28),iVar7);
          piVar3 = (int *)AoWE_TUnitList_GetUnit_23EDC2EF(uVar5,iVar6);
          uVar5 = (**(code **)(*piVar3 + 0x118))();
          piVar3 = (int *)AoWE_TUnitGFXResourceList_GetUnitGFXResource_23EDC2EF
                                    (*(undefined4 *)(*(int *)AoWE_AoWHSSet_678301D7_exref + 0x60),
                                     uVar5);
          (**(code **)(*piVar3 + 0x28))();
          iVar6 = iVar6 + 1;
          iVar2 = iVar2 + -1;
        } while (iVar2 != 0);
      }
      iVar7 = iVar7 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  return;
}

