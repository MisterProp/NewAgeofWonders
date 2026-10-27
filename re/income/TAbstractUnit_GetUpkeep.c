

int AoWE_TAbstractUnit_GetUpkeep_23EDC2EF(int *param_1)



{

  char cVar1;

  int iVar2;

  

                    /* 0x7fc64  1052  AoWE.TAbstractUnit.GetUpkeep@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x9b);

  if (cVar1 == '\0') {

    iVar2 = (**(code **)(*param_1 + 0xa0))();

    return (iVar2 + 1) * 2;

  }

  return 0;

}




