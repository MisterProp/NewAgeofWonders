

int AoWE_THero_ExperienceToLevel_23EDC2EF(undefined4 param_1,int param_2)



{

  int iVar1;

  int iVar2;

  int iVar3;

  

                    /* 0x876d4  651  AoWE.THero.ExperienceToLevel@23EDC2EF */

  iVar2 = 1;

  iVar3 = 0;

  iVar1 = AoWE_THero_GetLevelMax_23EDC2EF();

  do {

    iVar3 = iVar3 + (&DAT_558e84b8)[(iVar2 / 5) * 2];

    if (param_2 < iVar3) {

      return iVar2;

    }

    iVar2 = iVar2 + 1;

  } while (iVar2 < iVar1);

  return iVar2;

}




