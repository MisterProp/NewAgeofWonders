// /AoWEPACK.dpl : 557a8a64

undefined4 City_TCityProductionControl_LockQueueProduction_23EDC2EF(undefined4 param_1,int param_2)

{
  char cVar1;
  undefined4 uVar2;
  
                    /* 0xa8a64  6959  City.TCityProductionControl.LockQueueProduction@23EDC2EF */
  uVar2 = AoWE_TProductionControl_LockQueueProduction_23EDC2EF(param_1,param_2);
  cVar1 = City_ProductionIDToCityAction_CFDECB0A(*(undefined4 *)(param_2 + 8));
  if (cVar1 == '\x04') {
    uVar2 = 1;
  }
  else if (cVar1 == '\x05') {
    uVar2 = 1;
  }
  return uVar2;
}

