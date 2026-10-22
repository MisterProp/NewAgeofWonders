
void HSEngine_TMultiHexMO_OverrideTerrain_23EDC2EF(int *param_1)

{
  int iVar1;
  char cVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  undefined1 local_30 [8];
  int local_28;
  int local_20;
  int local_1c;
  
                    /* 0xb2d4  200  HSEngine.TMultiHexMO.OverrideTerrain@23EDC2EF */
  iVar1 = *(int *)(param_1[1] + 4);
  iVar4 = (**(code **)(*param_1 + 0x54))();
  HSEngine_InitHNWalk_3DC7E9B4(local_30,(int)(char)param_1[4],(int)*(char *)((int)param_1 + 0x11),0)
  ;
  while (local_28 < iVar4) {
    if ((((-1 < local_20) && (-1 < local_1c)) && (local_20 < *(int *)(iVar1 + 0xc))) &&
       (local_1c < *(int *)(iVar1 + 0x10))) {
      cVar3 = (**(code **)(*param_1 + 0x4c))(param_1,local_28);
      cVar2 = (**(code **)(*param_1 + 0x58))(param_1,local_28);
      iVar5 = HSEngine_TMapLevel_GetField_23EDC2EF(iVar1,local_20,local_1c);
      if (-1 < cVar3) {
        *(char *)(iVar5 + 0x14) = cVar3;
      }
      if (-1 < cVar2) {
        *(char *)(iVar5 + 0x15) = cVar2;
      }
    }
    HSEngine_WalkToNextHN_77BE26A7(local_30);
  }
  HSEngine_InitHNWalk_3DC7E9B4(local_30,(int)(char)param_1[4],(int)*(char *)((int)param_1 + 0x11),0)
  ;
  do {
    if (iVar4 <= local_28) {
      return;
    }
    cVar3 = (**(code **)(*param_1 + 0x4c))(param_1,local_28);
    if (cVar3 < '\0') {
      cVar3 = (**(code **)(*param_1 + 0x58))(param_1,local_28);
      if (((-1 < cVar3) && (-1 < local_20)) &&
         ((-1 < local_1c &&
          ((local_20 < *(int *)(iVar1 + 0xc) && (local_1c < *(int *)(iVar1 + 0x10)))))))
      goto LAB_5560b3b6;
    }
    else {
LAB_5560b3b6:
      HSEngine_TMapLevel_GetField_23EDC2EF(iVar1,local_20,local_1c);
      HSEngine_TMapField_TerrainChanged_23EDC2EF();
    }
    HSEngine_WalkToNextHN_77BE26A7(local_30);
  } while( true );
}

