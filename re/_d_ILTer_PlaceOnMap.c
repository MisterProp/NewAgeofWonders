
undefined4 ILTer_TILTerrainMO_PlaceOnMap_23EDC2EF(int *param_1)

{
  undefined1 uVar1;
  undefined4 uVar2;
  
                    /* 0x189c8  950  ILTer.TILTerrainMO.PlaceOnMap@23EDC2EF */
  uVar2 = HSEngine_TMultiHexMO_PlaceOnMap_23EDC2EF(param_1);
  if ((char)uVar2 != '\0') {
    uVar1 = (**(code **)(*param_1 + 0x134))
                      (param_1,*(undefined4 *)(param_1[1] + 4),(char)param_1[4],
                       *(undefined1 *)((int)param_1 + 0x11));
    *(undefined1 *)(param_1 + 6) = uVar1;
  }
  return uVar2;
}

