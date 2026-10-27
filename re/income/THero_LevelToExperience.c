

int AoWE_THero_LevelToExperience_23EDC2EF(undefined4 param_1,int param_2)



{

  int iVar1;

  int iVar2;

  

                    /* 0x876a0  652  AoWE.THero.LevelToExperience@23EDC2EF */

  iVar2 = 0;

  param_2 = param_2 + -1;

  if (0 < param_2) {

    iVar1 = 1;

    do {

      if (iVar1 / 5 < 6) {

        iVar2 = iVar2 + (&DAT_558e84b8)[(iVar1 / 5) * 2];

      }

      iVar1 = iVar1 + 1;

      param_2 = param_2 + -1;

    } while (param_2 != 0);

  }

  return iVar2;

}




