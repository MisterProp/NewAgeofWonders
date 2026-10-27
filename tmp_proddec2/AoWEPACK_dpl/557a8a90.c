// /AoWEPACK.dpl : 557a8a90

undefined4 City_TCityProductionControl_LoopableProduction_23EDC2EF(undefined4 param_1,int param_2)

{
  char cVar1;
  undefined4 uVar2;
  
                    /* 0xa8a90  6958  City.TCityProductionControl.LoopableProduction@23EDC2EF */
  uVar2 = AoWE_TProductionControl_LoopableProduction_23EDC2EF(param_1,param_2);
  cVar1 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(param_2 + 8));
  if (cVar1 == '\x02') {
    uVar2 = 0;
  }
  else if (cVar1 == '\x03') {
    uVar2 = 0;
  }
  else if (cVar1 == '\x04') {
    uVar2 = 0;
  }
  else if (cVar1 == '\x05') {
    uVar2 = 0;
  }
  return uVar2;
}

