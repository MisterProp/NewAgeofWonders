

int AoWE_TLeader_GetSpherePicks_23EDC2EF(int param_1,char param_2)



{

  int iVar1;

  int iVar2;

  

                    /* 0x8b17c  507  AoWE.TLeader.GetSpherePicks@23EDC2EF */

  if (param_2 == '\0') {

    iVar1 = 4;

  }

  else {

    iVar1 = 0;

    iVar2 = *(int *)(*(int *)(param_1 + 0xa0) + 8) + -1;

    if (-1 < iVar2) {

      do {

        if (*(char *)(*(int *)(*(int *)(param_1 + 0xa0) + 4) + iVar2) == param_2) {

          iVar1 = iVar1 + 1;

        }

        iVar2 = iVar2 + -1;

      } while (iVar2 != -1);

      return iVar1;

    }

  }

  return iVar1;

}




