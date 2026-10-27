// /AoWEPACK.dpl : 557bdc34

int PrdPlace_TProductionPlaceControl_UnitProductionManaUpkeep_23EDC2EF
              (int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  
                    /* 0xbdc34  7491
                       PrdPlace.TProductionPlaceControl.UnitProductionManaUpkeep@23EDC2EF */
  cVar1 = (**(code **)(**(int **)(param_1 + 0x28) + 0x230))();
  if (cVar1 != '\b') {
    iVar2 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),param_2);
    iVar2 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x5c),
                       *(undefined4 *)(iVar2 + 8));
    return *(char *)(iVar2 + 0x2f) * 5;
  }
  return 0;
}

