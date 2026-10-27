

void HSEngine_HVtoHX_8E9B8701(int param_1,int param_2,int *param_3,int *param_4)



{

  int iVar1;

  

                    /* 0xde84  75  HSEngine.HVtoHX@8E9B8701 */

  iVar1 = param_2 - param_1 >> 1;

  if (param_2 - param_1 < 0) {

    *param_3 = param_1 + param_2;

    *param_4 = iVar1;

  }

  else {

    *param_3 = param_1 + param_2;

    *param_4 = iVar1;

  }

  return;

}




