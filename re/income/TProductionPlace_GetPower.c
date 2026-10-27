

int PrdPlace_TProductionPlace_GetPower_23EDC2EF(int *param_1)



{

  char cVar1;

  undefined4 uVar2;

  int iVar3;

  uint uVar4;

  

                    /* 0xbe468  7472  PrdPlace.TProductionPlace.GetPower@23EDC2EF */

  if (((('\0' < (char)param_1[0xc]) && (cVar1 = (**(code **)(*param_1 + 0x230))(), cVar1 != '\0'))

      && (cVar1 = (**(code **)(*param_1 + 0x150))(), cVar1 == '\0')) &&

     (cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1), cVar1 == '\0')) {

    uVar2 = (**(code **)(*param_1 + 0x230))();

    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF

                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),

                       (int)(char)param_1[0xc]);

    uVar4 = AoWE_TLeader_GetSpherePicks_23EDC2EF(*(undefined4 *)(iVar3 + 0xd4),uVar2);

    if ((uVar4 & 0xff) != 0) {

      return (uVar4 & 0xff) * 5 + 10;

    }

    return 0;

  }

  return 0;

}




