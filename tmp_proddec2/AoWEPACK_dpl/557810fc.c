// /AoWEPACK.dpl : 557810fc

int AoWE_TAbstractUnit_Place_23EDC2EF
              (undefined4 param_1,undefined4 param_2,undefined4 param_3,byte param_4,char param_5,
              undefined4 param_6)

{
  char cVar1;
  int *piVar2;
  int iVar3;
  undefined1 local_28 [12];
  int local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_8;
  
                    /* 0x810fc  986  AoWE.TAbstractUnit.Place@23EDC2EF */
  iVar3 = 0;
  local_8 = param_1;
  HSEngine_InitHNWalk_3DC7E9B4(local_28,param_2,param_3,0);
  while ((iVar3 == 0 && (((local_1c == 0 || ((param_4 & 1) != 0)) && (local_1c < 0x1e))))) {
    cVar1 = AoWE_TAbstractUnit_CanPlace_23EDC2EF(local_8,local_18,local_14,param_4,param_5,param_6);
    if (cVar1 != '\0') {
      piVar2 = (int *)HSEngine_TMapContainer_GetField_23EDC2EF
                                (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x10),local_18,local_14,
                                 param_6);
      iVar3 = (**(code **)(*piVar2 + 0x80))(piVar2,0x20217);
      if (iVar3 == 0) {
        iVar3 = AoWE_TArmyHS_Create_23EDC2EF(&AoWE__TArmyHS_A111847B,1,0);
        if (param_5 != -1) {
          AoWE_TArmy_SetPlayer_23EDC2EF(*(undefined4 *)(iVar3 + 0x1c),param_5);
        }
        cVar1 = (**(code **)(**(int **)(iVar3 + 0x1c) + 0xac))(*(int **)(iVar3 + 0x1c),local_8);
        if ((cVar1 == '\0') ||
           (cVar1 = HSEngine_THSMap_PlaceHX_23EDC2EF
                              (AoWE_AoWHSMap_6B957A58,iVar3,local_18,param_6,local_14),
           cVar1 == '\0')) {
          System_TObject_Free_23EDC2EF(iVar3);
          iVar3 = 0;
        }
      }
      else {
        cVar1 = (**(code **)(**(int **)(iVar3 + 0x1c) + 0xac))(*(int **)(iVar3 + 0x1c),local_8);
        if (cVar1 == '\0') {
          iVar3 = 0;
        }
      }
    }
    if (iVar3 == 0) {
      HSEngine_WalkToNextHN_77BE26A7(local_28);
    }
  }
  return iVar3;
}

