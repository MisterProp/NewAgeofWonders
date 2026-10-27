// /AoWEPACK.dpl : 5575dc18

void AoWE_TProductionControl_ReadWrite_23EDC2EF(int param_1,int *param_2)

{
                    /* 0x5dc18  2328  AoWE.TProductionControl.ReadWrite@23EDC2EF */
  Engine_TEObject_ReadWrite_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x14),0x14);
  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x1c),0x15);
  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x24),0x16);
  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x20,1,0x17);
  (**(code **)(*param_2 + 0x38))(param_2,param_1 + 0x18,0x18);
  return;
}

