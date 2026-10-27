
char AoWE_TAoWHSMap_ValidateObservation_23EDC2EF
               (int *param_1,char param_2,char param_3,int param_4,char param_5,char param_6)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined1 local_2c [12];
  int local_20;
  int local_1c;
  int local_18;
  char local_9;
  int *local_8;
  
                    /* 0x75d5c  1463  AoWE.TAoWHSMap.ValidateObservation@23EDC2EF */
  if (((char)param_1[0x60] == '\0') || (param_2 == *(char *)(AoWE_AoWHSMap_6B957A58 + 0xa5))) {
    cVar1 = '\0';
  }
  else {
    cVar1 = '\0';
    local_9 = param_3;
    local_8 = param_1;
    iVar2 = HSEngine_TMapContainer_GetMapLevel_23EDC2EF(param_1[4],(int)param_5);
    HSEngine_InitHNWalk_3DC7E9B4(local_2c,(int)local_9,(int)param_6,0);
    while ((local_20 <= param_4 && (cVar1 == '\0'))) {
      if ((-1 < local_1c) &&
         (((-1 < local_18 && (local_1c < *(int *)(iVar2 + 0xc))) &&
          (local_18 < *(int *)(iVar2 + 0x10))))) {
        iVar3 = HSEngine_TMapLevel_GetField_23EDC2EF(iVar2,local_1c,local_18);
        if (((*(char *)(iVar3 + 0x1a) == '\0') && (*(char *)(AoWE_AoWHSMap_6B957A58 + 200) != '\0'))
           || ((cVar1 = AoWE_TAoWMapField_GetExplored_23EDC2EF
                                  (iVar3,(int)*(char *)(AoWE_AoWHSMap_6B957A58 + 0xa5)),
               cVar1 == '\0' && (*(char *)(AoWE_AoWHSMap_6B957A58 + 0xc9) != '\0')))) {
          cVar1 = '\0';
        }
        else {
          cVar1 = '\x01';
        }
      }
      if (cVar1 == '\0') {
        HSEngine_WalkToNextHN_77BE26A7(local_2c);
      }
    }
    if (cVar1 != '\0') {
      (**(code **)(*local_8 + 200))(local_8,local_1c,local_18,3,(int)param_5);
    }
  }
  return cVar1;
}

