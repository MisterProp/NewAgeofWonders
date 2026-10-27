

void AoWE_TPlayerStructureResource_ReadWrite_23EDC2EF(int *param_1,undefined4 *param_2)



{

  uint uVar1;

  

                    /* 0x61a44  2178  AoWE.TPlayerStructureResource.ReadWrite@23EDC2EF */

  AoWE_TStructureResource_ReadWrite_23EDC2EF(param_1,param_2);

  uVar1 = (**(code **)*param_2)();

  if ((uVar1 & 1) != 0) {

    (**(code **)(*param_1 + 0x7c))();

  }

  return;

}




