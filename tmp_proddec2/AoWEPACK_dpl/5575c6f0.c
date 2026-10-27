// /AoWEPACK.dpl : 5575c6f0

void AoWE_TProductionItem_ReadWrite_23EDC2EF(int param_1,int *param_2)

{
                    /* 0x5c6f0  2390  AoWE.TProductionItem.ReadWrite@23EDC2EF */
  Engine_TEObject_ReadWrite_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 8,0x14);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0xc,0x15);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x10,0x16);
  return;
}

