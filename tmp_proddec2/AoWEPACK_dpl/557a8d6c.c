// /AoWEPACK.dpl : 557a8d6c

void City_TCityProductionControl_GetLocation_23EDC2EF
               (int param_1,undefined1 *param_2,undefined1 *param_3,undefined1 *param_4)

{
  undefined1 uVar1;
  
                    /* 0xa8d6c  6954  City.TCityProductionControl.GetLocation@23EDC2EF */
  uVar1 = (**(code **)(**(int **)(param_1 + 0x28) + 0x74))();
  *param_2 = uVar1;
  uVar1 = (**(code **)(**(int **)(param_1 + 0x28) + 0x78))();
  *param_3 = uVar1;
  uVar1 = (**(code **)(**(int **)(param_1 + 0x28) + 0x7c))();
  *param_4 = uVar1;
  return;
}

