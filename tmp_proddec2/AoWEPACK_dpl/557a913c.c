// /AoWEPACK.dpl : 557a913c

int City_TCityProductionControl_UnitProductionTime_23EDC2EF(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  
                    /* 0xa913c  6949  City.TCityProductionControl.UnitProductionTime@23EDC2EF */
  iVar2 = AoWE_TUnitProductionList_GetItem_23EDC2EF(*(undefined4 *)(param_1 + 0x1c),param_2);
  iVar2 = AoWE_TUnitResourceList_GetUnitResource_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSSet_678301D7_558e92e8 + 0x5c),
                     *(undefined4 *)(iVar2 + 8));
  cVar1 = *(char *)(iVar2 + 0x2f);
  uVar3 = City_TCity_GetSize_23EDC2EF(*(undefined4 *)(param_1 + 0x28));
  iVar2 = (int)uVar3 >> 1;
  if (iVar2 < 0) {
    iVar2 = iVar2 + (uint)((uVar3 & 1) != 0);
  }
  iVar2 = (cVar1 - iVar2) + 1;
  if (iVar2 < 1) {
    iVar2 = 1;
  }
  return iVar2;
}

