

void PrdPlace_TProductionPlaceControl_GetLocation_23EDC2EF

               (int param_1,undefined1 *param_2,undefined1 *param_3,undefined1 *param_4)



{

  int *piVar1;

  undefined1 uVar2;

  

                    /* 0xbdc7c  7489  PrdPlace.TProductionPlaceControl.GetLocation@23EDC2EF */

  piVar1 = *(int **)(param_1 + 0x28);

  uVar2 = (**(code **)(*piVar1 + 0x74))();

  *param_2 = uVar2;

  uVar2 = (**(code **)(*piVar1 + 0x78))();

  *param_3 = uVar2;

  uVar2 = (**(code **)(*piVar1 + 0x7c))();

  *param_4 = uVar2;

  return;

}




