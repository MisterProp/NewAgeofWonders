

void Mine_TMine_SetRazed_23EDC2EF(int *param_1,char param_2)



{

  int iVar1;

  

                    /* 0xb4648  7166  Mine.TMine.SetRazed@23EDC2EF */

  if (param_2 == '\0') {

    *(byte *)(param_1 + 0xf) = *(byte *)(param_1 + 0xf) & 0xfe;

  }

  else {

    (**(code **)(*param_1 + 0x1f0))(param_1,0xffffffff);

    *(byte *)(param_1 + 0xf) = *(byte *)(param_1 + 0xf) | 1;

  }

  iVar1 = (**(code **)(*param_1 + 300))(param_1,(int)(char)param_1[6]);

  param_1[5] = iVar1;

  return;

}




