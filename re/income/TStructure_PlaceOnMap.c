

undefined4 AoWE_TStructure_PlaceOnMap_23EDC2EF(int *param_1)



{

  char cVar1;

  undefined4 uVar2;

  

                    /* 0x5e7c0  2301  AoWE.TStructure.PlaceOnMap@23EDC2EF */

  uVar2 = ILTer_TILTerrainMO_PlaceOnMap_23EDC2EF(param_1);

  cVar1 = (**(code **)(*param_1 + 0x84))();

  if (cVar1 == '\0') {

    AoWE_TStructure_UpdateRoads_23EDC2EF(param_1,param_1[1]);

  }

  return uVar2;

}




