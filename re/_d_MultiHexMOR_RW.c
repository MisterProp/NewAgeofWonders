
void HSEngine_TMultiHexMOR_ReadWrite_23EDC2EF(int param_1,int *param_2)

{
                    /* 0xb4ec  194  HSEngine.TMultiHexMOR.ReadWrite@23EDC2EF */
  HSEngine_TMapObjectResource_ReadWrite_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x34,0xc);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x38,0xd);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x3c,0xe);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x40,0xf);
  return;
}

