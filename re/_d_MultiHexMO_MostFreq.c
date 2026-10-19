
char HSEngine_TMultiHexMO_GetMostFrequentMapFieldTerrainType_23EDC2EF
               (int *param_1,int param_2,char param_3,char param_4,char param_5)

{
  int iVar1;
  char cVar2;
  int iVar3;
  undefined1 local_ac [8];
  int local_a4;
  int local_9c;
  int local_98;
  byte local_8a [128];
  char local_a;
  char local_9;
  int *local_8;
  
                    /* 0xb1ac  201  HSEngine.TMultiHexMO.GetMostFrequentMapFieldTerrainType@23EDC2EF
                        */
  local_a = param_4;
  iVar1 = *(int *)(param_1[2] + 0x28);
  if (param_4 < '\0') {
    local_a = '\0';
  }
  local_9 = param_3;
  local_8 = param_1;
  System__FillChar_51F89FF7(local_8a,0x80,0);
  HSEngine_InitHNWalk_3DC7E9B4(local_ac,(int)local_9,(int)param_5,0);
  do {
    if (*(int *)(iVar1 + 4) <= local_a4) {
      if (-1 < param_4) {
        if (local_8a[local_a] == local_8a[param_4]) {
          local_a = param_4;
        }
      }
      return local_a;
    }
    cVar2 = HSEngine_TTerrainList_GetTerrain_23EDC2EF(iVar1,local_a4);
    if (cVar2 < '\0') {
      cVar2 = (**(code **)(*local_8 + 0x58))(local_8,local_a4);
      if (-1 < cVar2) goto LAB_5560b227;
    }
    else {
LAB_5560b227:
      if ((((-1 < local_9c) && (-1 < local_98)) && (local_9c < *(int *)(param_2 + 0xc))) &&
         (local_98 < *(int *)(param_2 + 0x10))) {
        iVar3 = HSEngine_TMapLevel_GetField_23EDC2EF(param_2,local_9c,local_98);
        cVar2 = *(char *)(iVar3 + 0x14);
        if ((-1 < cVar2) &&
           (local_8a[cVar2] = local_8a[cVar2] + 1, local_8a[local_a] < local_8a[cVar2])) {
          local_a = cVar2;
        }
      }
    }
    HSEngine_WalkToNextHN_77BE26A7(local_ac);
  } while( true );
}

