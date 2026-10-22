
void HSEngine_TMultiHexMO_UpdateTerrain_23EDC2EF
               (int *param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  undefined1 local_2c [8];
  int local_24;
  int local_1c;
  int local_18;
  int local_c;
  undefined4 local_8;
  
                    /* 0xaed8  207  HSEngine.TMultiHexMO.UpdateTerrain@23EDC2EF */
  local_c = *(int *)(*(int *)(param_1[2] + 0x28) + 4);
  local_8 = param_3;
  HSEngine_InitHNWalk_3DC7E9B4(local_2c,param_3,param_4,0);
  do {
    if (local_24 == local_c) {
      return;
    }
    cVar1 = (**(code **)(*param_1 + 0x4c))(param_1,local_24);
    if (cVar1 < '\0') {
      cVar1 = (**(code **)(*param_1 + 0x58))(param_1,local_24);
      if (-1 < cVar1) goto LAB_5560af24;
    }
    else {
LAB_5560af24:
      if ((((-1 < local_1c) && (-1 < local_18)) && (local_1c < *(int *)(param_2 + 0xc))) &&
         (local_18 < *(int *)(param_2 + 0x10))) {
        HSEngine_TMapLevel_GetField_23EDC2EF(param_2,local_1c,local_18);
        HSEngine_TMapField_UpdateTerrain_23EDC2EF();
      }
    }
    HSEngine_WalkToNextHN_77BE26A7(local_2c);
  } while( true );
}

