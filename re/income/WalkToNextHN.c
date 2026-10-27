

void HSEngine_WalkToNextHN_77BE26A7(int *param_1)



{

  int iVar1;

  int iVar2;

  int iVar3;

  

                    /* 0xe610  54  HSEngine.WalkToNextHN@77BE26A7 */

  iVar1 = param_1[1];

  param_1[2] = param_1[2] + 1;

  iVar2 = *param_1;

  iVar3 = (param_1[4] & 1U) * 0x40;

  param_1[1] = iVar1 + 1;

  if (iVar1 + 1 < param_1[3]) {

    iVar1 = *(int *)(&DAT_5562e250 + iVar2 * 8 + iVar3);

    param_1[5] = *(int *)((int)(&DAT_5562e250 + iVar2 * 8 + iVar3) + 4) + param_1[5];

    param_1[4] = param_1[4] + iVar1;

    return;

  }

  if (iVar2 == 5) {

    param_1[3] = param_1[3] + 1;

    *param_1 = 0;

    param_1[1] = 0;

    param_1[4] = param_1[6];

    param_1[5] = param_1[7] - param_1[3];

    return;

  }

  *param_1 = iVar2 + 1;

  iVar1 = *(int *)(&DAT_5562e250 + iVar2 * 8 + iVar3);

  param_1[5] = *(int *)((int)(&DAT_5562e250 + iVar2 * 8 + iVar3) + 4) + param_1[5];

  param_1[4] = param_1[4] + iVar1;

  param_1[1] = 0;

  return;

}




