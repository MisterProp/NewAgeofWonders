
void AoWE_TUnitProduction_ReadWrite_23EDC2EF(int param_1,int *param_2)

{
                    /* 0x5c5b0  2396  AoWE.TUnitProduction.ReadWrite@23EDC2EF */
  Engine_TEObject_ReadWrite_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 8,0x14);
  (**(code **)(*param_2 + 0x38))(param_2,param_1 + 0xc,0x15);
  return;
}

