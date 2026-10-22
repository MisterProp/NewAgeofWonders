
undefined4
AoWE_TStructure_PlaceHX_23EDC2EF
          (int *param_1,undefined4 param_2,undefined4 param_3,undefined1 param_4,undefined1 param_5)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined3 uVar4;
  
                    /* 0x5ed10  2274  AoWE.TStructure.PlaceHX@23EDC2EF */
  cVar1 = (**(code **)(*param_1 + 0x84))();
  if ((cVar1 == '\0') &&
     (cVar1 = (**(code **)(*param_1 + 0x114))(param_1,param_2,param_3,param_4,param_5),
     cVar1 == '\0')) {
    return 0;
  }
  if (*(char *)(param_1[2] + 0x44) == '\0') {
    uVar4 = (undefined3)((uint)param_1[2] >> 8);
    uVar3 = ILTer_TILTerrainMO_PlaceHX_23EDC2EF
                      (param_1,param_2,param_3,CONCAT31(uVar4,param_4),CONCAT31(uVar4,param_5));
  }
  else {
    cVar1 = (**(code **)(*param_1 + 0x84))();
    if (cVar1 == '\0') {
      iVar2 = AoWE_TPadControl_PlacePad_23EDC2EF
                        (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x58),param_3,param_5,
                         *(undefined1 *)(param_1[2] + 0x46),param_4);
    }
    else {
      iVar2 = 0;
    }
    if ((iVar2 == 0) && (cVar1 = (**(code **)(*param_1 + 0x84))(), cVar1 == '\0')) {
      return 0;
    }
    if (iVar2 != 0) {
      *(undefined1 *)(param_1 + 6) = *(undefined1 *)(iVar2 + 0x18);
    }
    uVar3 = ILTer_TILTerrainMO_PlaceHX_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  }
  return uVar3;
}

