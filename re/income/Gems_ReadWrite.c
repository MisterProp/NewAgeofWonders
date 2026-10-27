
void Gems_TGems_ReadWrite_23EDC2EF(int param_1,int *param_2)

{
                    /* 0xc4fdc  7743  Gems.TGems.ReadWrite@23EDC2EF */
  HSEngine_TMultiHexMO_ReadWrite_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x1c,5);
  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x20,1,6);
  return;
}

