

void AoWE_TStructure_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

                    /* 0x5e658  2307  AoWE.TStructure.ReadWrite@23EDC2EF */

  HSEngine_TMultiHexMO_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x25,0);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x1c,0x62);

  (**(code **)(*param_2 + 0x44))(param_2,param_1 + 0x24,0xff,99);

  return;

}




