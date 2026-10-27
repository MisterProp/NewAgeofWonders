// /AoWEPACK.dpl : 557bdb24

/* WARNING: Removing unreachable block (ram,0x557bdba4) */

int PrdPlace_TProductionPlaceControl_UnitProductionTime_23EDC2EF(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  
                    /* 0xbdb24  7495  PrdPlace.TProductionPlaceControl.UnitProductionTime@23EDC2EF
                        */
  iVar2 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),param_2);
  iVar2 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x5c),
                     *(undefined4 *)(iVar2 + 8));
  iVar2 = *(char *)(iVar2 + 0x2f) * 2;
  cVar1 = (**(code **)(**(int **)(param_1 + 0x28) + 0x230))();
  if (cVar1 != '\b') {
    cVar1 = *(char *)(*(int *)(param_1 + 0x28) + 0x30);
    if (cVar1 < '\x01') {
      uVar5 = 0;
    }
    else {
      uVar3 = (**(code **)(**(int **)(param_1 + 0x28) + 0x230))();
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),
                         (int)cVar1);
      uVar5 = AoWE_TLeader_GetSpherePicks_23EDC2EF(*(undefined4 *)(iVar4 + 0xd4),uVar3);
      uVar5 = uVar5 & 0xff;
    }
    iVar2 = iVar2 - ((int)uVar5 >> 1);
    if (iVar2 < 1) {
      iVar2 = 1;
    }
  }
  return iVar2;
}

