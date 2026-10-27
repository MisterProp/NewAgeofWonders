
void AoWE_TProductionSettings_ReadWrite_23EDC2EF(int param_1,int *param_2)

{
                    /* 0x5c8c4  2383  AoWE.TProductionSettings.ReadWrite@23EDC2EF */
  Engine_TEObject_ReadWrite_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 8,10);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0xc,0xb);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x10,0xc);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x1c,0xd);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x1d,0xe);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x20,0xf);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x28,0x10);
  (**(code **)(*param_2 + 8))(param_2,param_1 + 0x24,0,0x11);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x14,0x12);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x18,0x13);
  return;
}

