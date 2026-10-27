

int AoWE_TPlayerMagicControl_GetNetPowerToMana_23EDC2EF

              (int param_1,undefined4 param_2,undefined4 param_3)



{

  char cVar1;

  int iVar2;

  int iVar3;

  

                    /* 0x7cee4  1194  AoWE.TPlayerMagicControl.GetNetPowerToMana@23EDC2EF */

  iVar2 = AoWE_TPlayerMagicControl_GetNetPower_23EDC2EF(param_1,param_2,param_3,param_3);

  iVar3 = AoWE_TPlayerMagicControl_GetNetPowerToResearch_23EDC2EF(param_1);

  iVar2 = iVar2 - iVar3;

  cVar1 = *(char *)(*(int *)(param_1 + 0x10) + 0xa7);

  if (cVar1 == '\x02') {

    iVar2 = System__ROUND_51F89FF7();

  }

  else if (cVar1 == '\x03') {

    iVar2 = System__ROUND_51F89FF7();

  }

  else if (cVar1 == '\x06') {

    iVar2 = System__ROUND_51F89FF7();

  }

  else if (cVar1 == '\a') {

    iVar2 = System__ROUND_51F89FF7();

  }

  return iVar2;

}




