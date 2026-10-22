
undefined4
ILTer_TILTerrainMO_PlaceHX_23EDC2EF
          (int *param_1,undefined4 param_2,uint param_3,char param_4,byte param_5)

{
  undefined1 uVar1;
  char cVar2;
  undefined4 uVar3;
  uint uVar4;
  undefined4 uVar5;
  int iVar6;
  
                    /* 0x18b80  947  ILTer.TILTerrainMO.PlaceHX@23EDC2EF */
  uVar3 = HSEngine_TMultiHexMO_PlaceHX_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  if ((char)uVar3 != '\0') {
    uVar3 = 0;
    uVar4 = (uint)param_5;
    uVar5 = HSEngine_TMapContainer_GetMapLevel_23EDC2EF(param_2,(int)param_4);
    uVar1 = (**(code **)(*param_1 + 0x134))(param_1,uVar5,param_3 & 0xff,uVar4);
    *(undefined1 *)(param_1 + 6) = uVar1;
    if (-1 < (char)param_1[6]) {
      iVar6 = (**(code **)(*param_1 + 300))(param_1,(int)(char)param_1[6]);
      param_1[5] = iVar6;
      if (iVar6 != -1) {
        cVar2 = (**(code **)(*param_1 + 0x84))();
        if (cVar2 == '\0') {
          HSEngine_TMultiHexMO_OverrideTerrain_23EDC2EF(param_1);
        }
        uVar3 = 1;
      }
    }
  }
  return uVar3;
}

