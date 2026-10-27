

void City_TCityResource_ShowCity_23EDC2EF

               (int param_1,int param_2,int param_3,int *param_4,undefined4 param_5)



{

  int *piVar1;

  char cVar2;

  

                    /* 0xade3c  6820  City.TCityResource.ShowCity@23EDC2EF */

  param_2 = *(int *)(param_1 + 0x34) + param_2;

  param_3 = param_3 + *(int *)(param_1 + 0x38);

  piVar1 = (int *)param_4[0x1a];

  if (piVar1 != (int *)0x0) {

    (**(code **)(*piVar1 + 0x84))(piVar1,param_2,param_3,param_5);

  }

  if (param_4[0x1c] != 0) {

    cVar2 = (**(code **)(*param_4 + 0x70))();

    if ((cVar2 == '\0') ||

       ((*(uint *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x40) & 6) != 6)) {

      (**(code **)(*(int *)param_4[0x1c] + 0x84))((int *)param_4[0x1c],param_2,param_3,param_5);

    }

    else {

      (**(code **)(*(int *)param_4[0x1c] + 0x9c))((int *)param_4[0x1c],param_2,param_3,param_5);

    }

  }

  piVar1 = (int *)param_4[0x1b];

  if (piVar1 != (int *)0x0) {

    (**(code **)(*piVar1 + 0x84))(piVar1,param_2,param_3,param_5);

  }

  return;

}




