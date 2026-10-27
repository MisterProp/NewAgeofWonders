

void Crops_TCrop_Deactivate_23EDC2EF(int param_1)



{

  int *piVar1;

  

                    /* 0xa5e40  6756  Crops.TCrop.Deactivate@23EDC2EF */

  piVar1 = *(int **)(param_1 + 0x1c);

  if ((piVar1 != (int *)0x0) &&

     ((*(byte *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x3c) & 8) == 0)) {

    *(undefined4 *)(param_1 + 0x1c) = 0;

    (**(code **)(*piVar1 + 0x23c))();

  }

  HSEngine_TMapObject_Deactivate_23EDC2EF(param_1);

  return;

}




