

void City_TCity_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

                    /* 0xacbcc  6850  City.TCity.ReadWrite@23EDC2EF */

  AoWE_TPlayerStructure_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x48,0xc);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x45,0xd);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x4c,1,0xf);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x4d,0x10);

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x50),0x12);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x44,1,0x16);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x74,1,0x1b);

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x58),0x1d);

  return;

}




