// /AoWEPACK.dpl : 5575d588

undefined4 AoWE_TProductionControl_CreateUnitProduction_23EDC2EF(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
                    /* 0x5d588  2349  AoWE.TProductionControl.CreateUnitProduction@23EDC2EF */
  iVar1 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),param_2);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = AoWE_TAbstractUnit_Create_23EDC2EF(&AoWE__TUnit_53822AF1,1,0);
    iVar1 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),param_2);
    uVar3 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                      (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x5c),*(undefined4 *)(iVar1 + 8));
    AoWE_TUnit_SetUnitResource_23EDC2EF(uVar2,uVar3);
  }
  return uVar2;
}

