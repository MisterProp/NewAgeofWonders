

void HSEngine_CenterHNtoHX_A52F39E4(uint param_1,int param_2,int param_3,int *param_4,uint *param_5)



{

  int iVar1;

  uint uVar2;

  int iVar3;

  

                    /* 0xe17c  68  HSEngine.CenterHNtoHX@A52F39E4 */

  if (param_3 == 0) {

    *param_5 = 0;

    *param_4 = 0;

  }

  else {

    iVar1 = System__TRUNC_51F89FF7();

    uVar2 = (iVar1 + 3) / 6;

    iVar3 = param_3 + (uVar2 - 1) * uVar2 * -3 + -1;

    iVar1 = iVar3 % (int)uVar2;

    switch(iVar3 / (int)uVar2) {

    case 0:

      HSEngine_HVtoHX_8E9B8701(uVar2,iVar1 - uVar2,param_5,param_4);

      break;

    case 1:

      *param_5 = uVar2;

      iVar3 = (int)(uVar2 + 1) >> 1;

      if (iVar3 < 0) {

        iVar3 = iVar3 + (uint)((uVar2 + 1 & 1) != 0);

      }

      *param_4 = iVar1 - iVar3;

      break;

    case 2:

      HSEngine_HVtoHX_8E9B8701(-iVar1,uVar2,param_5,param_4);

      break;

    case 3:

      HSEngine_HVtoHX_8E9B8701(-uVar2,uVar2 - iVar1,param_5,param_4);

      break;

    case 4:

      *param_5 = -uVar2;

      iVar3 = (int)uVar2 >> 1;

      if (iVar3 < 0) {

        iVar3 = iVar3 + (uint)((uVar2 & 1) != 0);

      }

      *param_4 = iVar3 - iVar1;

      break;

    case 5:

      HSEngine_HVtoHX_8E9B8701(iVar1,-uVar2,param_5,param_4);

    }

  }

  if (((param_1 & 1) == 0) || ((*param_5 & 1) == 0)) {

    *param_4 = *param_4 + param_2;

  }

  else {

    *param_4 = *param_4 + param_2 + 1;

  }

  *param_5 = *param_5 + param_1;

  return;

}




