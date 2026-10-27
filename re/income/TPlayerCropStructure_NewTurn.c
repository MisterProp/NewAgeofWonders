

void Crops_TPlayerCropStructure_NewTurn_23EDC2EF(int *param_1,undefined4 param_2)



{

  char cVar1;

  

                    /* 0xa6200  6738  Crops.TPlayerCropStructure.NewTurn@23EDC2EF */

  AoWE_TPlayerStructure_NewTurn_23EDC2EF(param_1,param_2);

  if ((char)param_2 == (char)param_1[0xc]) {

    cVar1 = (**(code **)(*param_1 + 0x228))();

    if (cVar1 != '\0') {

      (**(code **)(*param_1 + 0x22c))();

    }

  }

  return;

}




