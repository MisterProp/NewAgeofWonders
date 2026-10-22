
char ILTer_TILTerrainMO_CanPlace_23EDC2EF
               (int *param_1,int param_2,undefined4 param_3,char param_4,char param_5)

{
  undefined1 uVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined1 local_38 [8];
  int local_30;
  int local_28;
  int local_24;
  int local_18;
  int local_14;
  char local_d;
  int local_c;
  char local_6;
  char local_5;
  
                    /* 0x18a04  948  ILTer.TILTerrainMO.CanPlace@23EDC2EF */
  local_5 = (char)param_3;
  local_6 = '\x01';
  local_c = *(int *)(*(int *)(param_1[2] + 0x28) + 4);
  uVar3 = HSEngine_TMapContainer_GetMapLevel_23EDC2EF
                    (param_2,(int)param_4,param_3,CONCAT31((int3)((uint)local_c >> 8),param_5));
  uVar1 = (**(code **)(*param_1 + 0x134))(param_1,uVar3,local_5);
  *(undefined1 *)(param_1 + 6) = uVar1;
  HSEngine_InitHNWalk_3DC7E9B4(local_38,(int)local_5,(int)param_5,0);
  while ((local_6 != '\0' && (local_30 < local_c))) {
    local_d = (**(code **)(*param_1 + 0x4c))(param_1,local_30);
    cVar2 = (**(code **)(*param_1 + 0x58))(param_1,local_30);
    if (((-1 < local_d) || (-1 < cVar2)) &&
       ((((-1 < local_28 && (-1 < local_24)) && (local_28 < *(int *)(param_2 + 0xc))) &&
        (local_24 < *(int *)(param_2 + 0x10))))) {
      HSEngine_TMapContainer_GetField_23EDC2EF(param_2,local_28,local_24,(int)param_4);
      iVar4 = HSEngine_TMapField_GetMaxLevelNoneTransparentHS_23EDC2EF();
      if ((iVar4 == 0) || (cVar2 = (**(code **)(*param_1 + 0x120))(param_1,iVar4), cVar2 != '\0')) {
        local_6 = '\x01';
      }
      else {
        local_6 = '\0';
      }
    }
    HSEngine_WalkToNextHN_77BE26A7(local_38);
  }
  if (local_6 != '\0') {
    puVar5 = (undefined4 *)System_TObject_Create_23EDC2EF(&HSEngine__TTerrainList_7EE8CC88,1);
    (**(code **)*puVar5)(puVar5,*(undefined4 *)(param_1[2] + 0x28));
    if (-1 < puVar5[1] + -1) {
      local_14 = 0;
      local_18 = puVar5[1];
      do {
        cVar2 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(puVar5,local_14);
        if (-1 < cVar2) {
          HSEngine_TTerrainList_SetTerrain_23EDC2EF(puVar5,local_14,(char)param_1[6]);
        }
        local_14 = local_14 + 1;
        local_18 = local_18 + -1;
      } while (local_18 != 0);
    }
    if ((local_6 == '\0') ||
       (cVar2 = HSEngine_THSMap_CanChangeTerrain_23EDC2EF
                          (*(undefined4 *)(param_2 + 0x18),param_1,(int)local_5,puVar5,(int)param_4,
                           (int)param_5), cVar2 == '\0')) {
      local_6 = '\0';
    }
    else {
      local_6 = '\x01';
    }
    System_TObject_Free_23EDC2EF(puVar5);
  }
  return local_6;
}

