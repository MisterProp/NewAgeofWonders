// /AoWEPACK.dpl : 557a96a8

undefined4 City_TCity_GetActiveAction_23EDC2EF(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
                    /* 0xa96a8  6939  City.TCity.GetActiveAction@23EDC2EF */
  if (*(char *)(*(int *)(param_1 + 0x50) + 0x20) == '\x02') {
    iVar1 = AoWE_TProductionQueue_GetItem_23EDC2EF
                      (*(undefined4 *)(*(int *)(param_1 + 0x50) + 0x14),0);
    uVar2 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(iVar1 + 8));
    return uVar2;
  }
  return 0;
}

