// /AoWEPACK.dpl : 55784de4

void AoWE_TUnitResource_ReadWrite_23EDC2EF(int param_1,int *param_2)

{
  uint uVar1;
  byte bVar2;
  
                    /* 0x84de4  772  AoWE.TUnitResource.ReadWrite@23EDC2EF */
  AoWE_TAbstractUnitResource_ReadWrite_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x1c,10);
  (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x24,0xb);
  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x28,1,0xc);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x20,0xd);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x29,0xe);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2a,0xf);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2b,0x10);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2c,0x11);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2d,0x12);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2e,0x13);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2f,0x14);
  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x30,1,0x15);
  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x21,1,0x16);
  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x31,1,0x17);
  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x44,0x18);
  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x38),0x19);
  (**(code **)(*param_2 + 0x24))(param_2,*(undefined4 *)(param_1 + 0x48),0x1a);
  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x4c,0x1b);
  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x50,1,0x1c);
  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x51,1,0x1d);
  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x3c),0x1e);
  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x40),0x1f);
  uVar1 = (**(code **)*param_2)();
  if ((uVar1 & 1) != 0) {
    bVar2 = 0;
    do {
      AoWE_TAbilityOwner_ValidateOwnerTypeAbilities_23EDC2EF
                (*(undefined4 *)(param_1 + 0x38 + (uint)bVar2 * 4));
      bVar2 = bVar2 + 1;
    } while (bVar2 != 3);
  }
  return;
}

