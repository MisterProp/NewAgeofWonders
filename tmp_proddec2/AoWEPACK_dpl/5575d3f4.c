// /AoWEPACK.dpl : 5575d3f4

undefined4
AoWE_TProductionControl_ExecuteAIProduction_23EDC2EF
          (int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
                    /* 0x5d3f4  2354  AoWE.TProductionControl.ExecuteAIProduction@23EDC2EF */
  (**(code **)(*param_1 + 0x6c))();
  iVar1 = Engine_TEObject_Create_23EDC2EF(&AoWE__TProductionItem_E3617239,1,0);
  *(undefined4 *)(iVar1 + 8) = 1;
  *(undefined4 *)(iVar1 + 0xc) = param_2;
  *(undefined4 *)(iVar1 + 0x10) = param_3;
  (**(code **)(*param_1 + 100))(param_1,iVar1);
  System_TObject_Free_23EDC2EF(iVar1);
  return 1;
}

