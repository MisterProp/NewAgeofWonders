

int HSEngine_HXtoHN_500B4C01(uint param_1,int param_2)



{

  uint uVar1;

  int iVar2;

  int iVar3;

  uint extraout_EDX;

  uint local_14;

  uint local_10;

  

                    /* 0xdf88  70  HSEngine.HXtoHN@500B4C01 */

  uVar1 = HSEngine_HXtoRad_500B4C01(param_1,param_2);

  if (uVar1 == 0) {

    iVar2 = 0;

  }

  else {

    HSEngine_HXtoHV_8E9B8701(param_1,param_2,&local_14,&local_10);

    if (uVar1 == local_14) {

      iVar2 = 0;

      local_14 = local_10 + uVar1;

    }

    else if (uVar1 == local_10) {

      iVar2 = 2;

      local_14 = -local_14;

    }

    else if (-local_14 == uVar1) {

      iVar2 = 3;

      local_14 = uVar1 - local_10;

    }

    else if (-local_10 == uVar1) {

      iVar2 = 5;

    }

    else if (param_1 == uVar1) {

      iVar2 = 1;

      iVar3 = (int)(param_1 + 1) >> 1;

      if (iVar3 < 0) {

        iVar3 = iVar3 + (uint)((param_1 + 1 & 1) != 0);

      }

      local_14 = iVar3 + param_2;

    }

    else if (uVar1 == -param_1) {

      iVar2 = 4;

      iVar3 = (int)param_1 >> 1;

      if (iVar3 < 0) {

        iVar3 = iVar3 + (uint)((param_1 & 1) != 0);

      }

      local_14 = -iVar3 - param_2;

    }

    else {

      SysUtils_Exception_Create_23EDC2EF

                (SysUtils__Exception_9880D0BF_exref,CONCAT31((int3)(-uVar1 >> 8),1),

                 "arithemetic error!");

      iVar2 = System__RaiseExcept_51F89FF7();

      local_14 = extraout_EDX;

    }

    iVar2 = local_14 + (uVar1 - 1) * uVar1 * 3 + 1 + iVar2 * uVar1;

  }

  return iVar2;

}




