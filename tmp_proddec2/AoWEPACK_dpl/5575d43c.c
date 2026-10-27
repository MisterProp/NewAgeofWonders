// /AoWEPACK.dpl : 5575d43c

undefined4 AoWE_TProductionControl_ExecuteAIResearch_23EDC2EF(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined3 uVar3;
  
                    /* 0x5d43c  2353  AoWE.TProductionControl.ExecuteAIResearch@23EDC2EF */
  iVar1 = AoWE_TUnitProductionList_GetItem_23EDC2EF(param_1[7],param_2);
  if (*(char *)(iVar1 + 0xc) == '\0') {
    (**(code **)(*param_1 + 0x6c))();
    iVar1 = Engine_TEObject_Create_23EDC2EF(&AoWE__TProductionItem_E3617239,1,0);
    *(undefined4 *)(iVar1 + 8) = 2;
    *(undefined4 *)(iVar1 + 0xc) = param_2;
    (**(code **)(*param_1 + 100))(param_1,iVar1);
    uVar2 = System_TObject_Free_23EDC2EF(iVar1);
    uVar3 = (undefined3)((uint)uVar2 >> 8);
  }
  else {
    uVar3 = (undefined3)((uint)iVar1 >> 8);
  }
  return CONCAT31(uVar3,1);
}

