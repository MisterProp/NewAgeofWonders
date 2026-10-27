

void City_TCity_ResetEnchantments_23EDC2EF(int param_1)



{

                    /* 0xa98e0  6931  City.TCity.ResetEnchantments@23EDC2EF */

  City_TCity_UnregisterEnchantments_23EDC2EF(param_1);

  (**(code **)(**(int **)(param_1 + 0x58) + 0x30))();

  *(undefined1 *)(param_1 + 0x5c) = 0;

  return;

}




