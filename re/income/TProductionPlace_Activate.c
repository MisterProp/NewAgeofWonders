

void PrdPlace_TProductionPlace_Activate_23EDC2EF(int *param_1)



{

  char cVar1;

  int iVar2;

  undefined4 uVar3;

  

                    /* 0xbee9c  7448  PrdPlace.TProductionPlace.Activate@23EDC2EF */

  AoWE_TPlayerStructure_Activate_23EDC2EF(param_1);

  (**(code **)(*(int *)param_1[0x11] + 0xa0))();

  if ((char)param_1[0xc] != -1) {

    cVar1 = (**(code **)(*param_1 + 0x230))();

    if (cVar1 == '\b') {

      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                         (int)(char)param_1[0xc]);

      Classes_TList_Add_23EDC2EF(*(undefined4 *)(iVar2 + 0x24),param_1[0x11]);

    }

    if (param_1[0xf] != 0) {

      iVar2 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                         (int)(char)param_1[0xc]);

      AoWE_TPlayerMagicControl_RegisterPowerSource_23EDC2EF

                (*(undefined4 *)(iVar2 + 0x54),param_1[0xf]);

    }

    if (param_1[0x10] != 0) {

      uVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                        (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                         (int)(char)param_1[0xc]);

      AoWE_TPlayer_RegisterIncomeSource_23EDC2EF(uVar3,param_1[0x10]);

    }

  }

  return;

}




