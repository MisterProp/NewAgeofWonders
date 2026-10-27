

void AoWE_TStructureResource_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

  uint uVar1;

  

                    /* 0x60554  2226  AoWE.TStructureResource.ReadWrite@23EDC2EF */

  HSEngine_TMultiHexMOR_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0x38))(param_2,param_1 + 0x4c,0x14);

  uVar1 = (**(code **)*param_2)();

  if ((uVar1 & 4) == 0) {

    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x48,0x15);

  }

  (**(code **)(*param_2 + 0x24))(param_2,*(undefined4 *)(param_1 + 0x50),0x16);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x54,0x17);

  (**(code **)(*param_2 + 0x18))

            (param_2,0x1c,*param_2,AoWE_TUnitIndexListCollection_SaveToStream_23EDC2EF,

             *(undefined4 *)(param_1 + 0x5c),AoWE_TUnitIndexListCollection_LoadFromStream_23EDC2EF,

             *(undefined4 *)(param_1 + 0x5c));

  uVar1 = (**(code **)*param_2)();

  if ((uVar1 & 1) != 0) {

    AoWE_TStructureResource_CalculatePadIndex_23EDC2EF(param_1);

  }

  return;

}




