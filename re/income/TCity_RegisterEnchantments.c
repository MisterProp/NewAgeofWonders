

void City_TCity_RegisterEnchantments_23EDC2EF(int param_1)



{

  undefined1 uVar1;

  int iVar2;

  int iVar3;

  int *piVar4;

  

                    /* 0xa9860  6933  City.TCity.RegisterEnchantments@23EDC2EF */

  iVar2 = (**(code **)(**(int **)(param_1 + 0x58) + 0x54))();

  iVar2 = iVar2 + -1;

  if (-1 < iVar2) {

    do {

      iVar3 = AoWE_TGlobalEnchantmentList_GetEnchantments_23EDC2EF

                        (*(undefined4 *)(param_1 + 0x58),iVar2);

      uVar1 = *(undefined1 *)(iVar3 + 0x19);

      piVar4 = (int *)AoWE_TGlobalEnchantmentList_GetEnchantments_23EDC2EF

                                (*(undefined4 *)(param_1 + 0x58),iVar2);

      (**(code **)(*piVar4 + 0x68))(piVar4,uVar1);

      iVar2 = iVar2 + -1;

    } while (iVar2 != -1);

  }

  City_TCity_UpdateActiveEnchantments_23EDC2EF(param_1);

  return;

}




