// /AoWEPACK.dpl : 557bf340

void PrdPlace_TProductionPlace_ResetProductions_23EDC2EF(int *param_1)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
                    /* 0xbf340  7441  PrdPlace.TProductionPlace.ResetProductions@23EDC2EF */
  (**(code **)(**(int **)(param_1[0x11] + 0x1c) + 0x30))();
  cVar1 = (**(code **)(*param_1 + 0x230))();
  if (cVar1 == '\b') {
    iVar3 = param_1[2];
    iVar4 = *(int *)(*(int *)(iVar3 + 0x74) + 8);
    if (-1 < iVar4 + -1) {
      iVar5 = 0;
      do {
        uVar2 = Engine_TIntegerList_Get_23EDC2EF(*(undefined4 *)(iVar3 + 0x74),iVar5);
        AoWE_TUnitProductionList_Add_23EDC2EF(*(undefined4 *)(param_1[0x11] + 0x1c),uVar2,0);
        iVar5 = iVar5 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
    AoWE_TUnitProductionList_SortOnLevel_23EDC2EF(*(undefined4 *)(param_1[0x11] + 0x1c));
    iVar3 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1[0x11] + 0x1c),0);
    *(undefined1 *)(iVar3 + 0xc) = 1;
  }
  return;
}

