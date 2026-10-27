// /AoWEPACK.dpl : 557a8da0

int City_TCityProductionControl_CreateUnitProduction_23EDC2EF(int param_1)

{
  int iVar1;
  
                    /* 0xa8da0  6953  City.TCityProductionControl.CreateUnitProduction@23EDC2EF */
  iVar1 = AoWE_TProductionControl_CreateUnitProduction_23EDC2EF(param_1);
  if ((iVar1 != 0) && ((*(byte *)(*(int *)(param_1 + 0x28) + 0x5c) & 8) != 0)) {
    AoWE_TUnit_SetRank_23EDC2EF(iVar1,1);
  }
  return iVar1;
}

