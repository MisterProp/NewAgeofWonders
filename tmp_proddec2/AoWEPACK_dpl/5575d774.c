// /AoWEPACK.dpl : 5575d774

void AoWE_TProductionControl_UnitResearched_23EDC2EF(int param_1)

{
  int iVar1;
  
                    /* 0x5d774  2340  AoWE.TProductionControl.UnitResearched@23EDC2EF */
  iVar1 = AoWE_TUnitProductionList_GetItem_23EDC2EF
                    (*(undefined4 *)(param_1 + 0x1c),
                     *(undefined4 *)(*(int *)(param_1 + 0x24) + 0x28));
  *(undefined1 *)(iVar1 + 0xc) = 1;
  return;
}

