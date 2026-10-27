

void City_TCity_SetRazed_23EDC2EF(int *param_1,char param_2)



{

  char cVar1;

  int iVar2;

  

                    /* 0xab47c  6877  City.TCity.SetRazed@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x150))();

  if (param_2 != cVar1) {

    (**(code **)(*(int *)param_1[0x14] + 0x6c))();

    if (param_2 == '\0') {

      *(byte *)(param_1 + 0x11) = *(byte *)(param_1 + 0x11) & 0xfd;

    }

    else {

      (**(code **)(*param_1 + 0x1f0))(param_1,0xffffffff);

      *(byte *)(param_1 + 0x11) = *(byte *)(param_1 + 0x11) | 2;

    }

    iVar2 = (**(code **)(*param_1 + 300))(param_1,(int)(char)param_1[6]);

    param_1[5] = iVar2;

    City_TCity_ResetEnchantments_23EDC2EF(param_1);

    City_TCity_UpdateImages_23EDC2EF(param_1);

    (**(code **)(*param_1 + 0x234))();

    (**(code **)(*param_1 + 0x1c8))();

    City_TCity_SetWallType_23EDC2EF(param_1,0);

  }

  return;

}




