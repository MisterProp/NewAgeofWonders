

void PowerNode_TPowerNode_Activate_23EDC2EF(int param_1)



{

  int iVar1;

  

                    /* 0xd05fc  8158  PowerNode.TPowerNode.Activate@23EDC2EF */

  AoWE_TPlayerStructure_Activate_23EDC2EF(param_1);

  if (*(char *)(param_1 + 0x30) != -1) {

    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                       (int)*(char *)(param_1 + 0x30));

    AoWE_TPlayerMagicControl_RegisterPowerSource_23EDC2EF

              (*(undefined4 *)(iVar1 + 0x54),*(undefined4 *)(param_1 + 0x40));

  }

  return;

}




