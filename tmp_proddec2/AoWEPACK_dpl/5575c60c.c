// /AoWEPACK.dpl : 5575c60c

void AoWE_TUnitProductionList_Add_23EDC2EF(undefined4 param_1,undefined4 param_2,undefined1 param_3)

{
  int iVar1;
  
                    /* 0x5c60c  2394  AoWE.TUnitProductionList.Add@23EDC2EF */
  iVar1 = Engine_TEObject_Create_23EDC2EF(&AoWE__TUnitProduction_1D597496,1,param_1);
  *(undefined4 *)(iVar1 + 8) = param_2;
  *(undefined1 *)(iVar1 + 0xc) = param_3;
  return;
}

