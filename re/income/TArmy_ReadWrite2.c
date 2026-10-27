

void AoWE_TArmy_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

  uint uVar1;

  

                    /* 0x8e8ec  363  AoWE.TArmy.ReadWrite@23EDC2EF */

  Engine_TEObject_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x12,4);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x13,5);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x14,6);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x15,7);

  uVar1 = (**(code **)*param_2)();

  if ((((uVar1 & 2) == 0) || (AoWE_AoWHSMap_6B957A58 == 0)) ||

     (*(char *)(AoWE_AoWHSMap_6B957A58 + 0x13a) != '\x01')) {

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x25,8);

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x26,9);

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x27,10);

    (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x28,1,0xd);

  }

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x24,1,0xb);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x11,1,0xc);

  uVar1 = (**(code **)*param_2)();

  if (((uVar1 & 1) != 0) && ((*(byte *)(param_1 + 0x11) & 4) != 0)) {

    *(byte *)(param_1 + 0x11) = *(byte *)(param_1 + 0x11) & 0xfb;

  }

  return;

}




