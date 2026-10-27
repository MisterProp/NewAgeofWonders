// /AoWEPACK.dpl : 5575d5dc

undefined4 AoWE_TProductionControl_UnitProductionGold_23EDC2EF(int param_1,undefined4 param_2)

{
  int iVar1;
  
                    /* 0x5d5dc  2348  AoWE.TProductionControl.UnitProductionGold@23EDC2EF */
  iVar1 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),param_2);
  iVar1 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                    (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x5c),*(undefined4 *)(iVar1 + 8));
  return *(undefined4 *)(iVar1 + 0x4c);
}

