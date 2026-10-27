

undefined4 PrdPlace_TProductionPlaceControl_ExecuteProduction_23EDC2EF(int *param_1,int param_2)



{

  int iVar1;

  undefined1 uVar2;

  undefined4 uVar3;

  

                    /* 0xbe050  7483  PrdPlace.TProductionPlaceControl.ExecuteProduction@23EDC2EF */

  uVar3 = AoWE_TProductionControl_ExecuteProduction_23EDC2EF(param_1,param_2);

  if (((char)uVar3 != '\0') && (*(int *)(param_2 + 8) == 10)) {

    iVar1 = param_1[9];

    *(undefined1 *)(iVar1 + 0x1c) = *(undefined1 *)(param_2 + 0xc);

    *(undefined1 *)(iVar1 + 0x10) = 1;

    uVar2 = (**(code **)(*param_1 + 0x7c))();

    *(undefined1 *)(param_1[9] + 0x1d) = uVar2;

    *(undefined1 *)(param_1 + 8) = 2;

  }

  return uVar3;

}




