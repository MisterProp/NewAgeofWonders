// /AoWEPACK.dpl : 5575cbb0

int AoWE_TProductionControl_GetProduceUnitLevelMax_23EDC2EF(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
                    /* 0x5cbb0  2370  AoWE.TProductionControl.GetProduceUnitLevelMax@23EDC2EF */
  iVar3 = 0;
  iVar1 = (**(code **)(**(int **)(param_1 + 0x1c) + 0x54))();
  iVar1 = iVar1 + -1;
  if (-1 < iVar1) {
    do {
      iVar2 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),iVar1);
      if (*(char *)(iVar2 + 0xc) != '\0') {
        iVar2 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),iVar1);
        if (-1 < *(int *)(iVar2 + 8)) {
          iVar2 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),iVar1);
          iVar2 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                            (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x5c),
                             *(undefined4 *)(iVar2 + 8));
          if ((iVar2 != 0) && (iVar3 < *(char *)(iVar2 + 0x2f))) {
            iVar3 = (int)*(char *)(iVar2 + 0x2f);
          }
        }
      }
      iVar1 = iVar1 + -1;
    } while (iVar1 != -1);
  }
  return iVar3;
}

