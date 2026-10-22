
char Crops_TPlayerCropStructure_CanPlace_23EDC2EF
               (int *param_1,undefined4 param_2,undefined4 param_3,char param_4,char param_5)

{
  char cVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  char local_9;
  int *local_8;
  
                    /* 0xa6904  6723  Crops.TPlayerCropStructure.CanPlace@23EDC2EF */
  local_9 = (char)param_3;
  local_8 = param_1;
  cVar1 = AoWE_TStructure_CanPlace_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  if (cVar1 != '\0') {
    cVar2 = (**(code **)(*local_8 + 0x84))();
    if (cVar2 == '\0') {
      iVar3 = HSEngine_TMapContainer_GetMapLevel_23EDC2EF(param_2,(int)param_4);
      local_10 = *(int *)(*(int *)(local_8[2] + 0x74) + 8);
      while ((cVar1 != '\0' && (local_10 != 0))) {
        local_10 = local_10 + -1;
        HSEngine_CenterHNtoHX_A52F39E4
                  ((int)local_9,(int)param_5,
                   *(undefined2 *)(*(int *)(*(int *)(local_8[2] + 0x74) + 4) + local_10 * 2),
                   &local_18,&local_14);
        local_1c = local_14;
        local_20 = local_18;
        iVar6 = 0;
        while ((cVar1 != '\0' && (iVar6 != 7))) {
          if ((-1 < local_1c) &&
             (((local_1c < *(int *)(iVar3 + 0xc) && (-1 < local_20)) &&
              (local_20 < *(int *)(iVar3 + 0x10))))) {
            uVar4 = HSEngine_TMapLevel_GetField_23EDC2EF(iVar3,local_1c,local_20);
            iVar5 = HSEngine_TMapField_FindNoneTransparentHS_23EDC2EF
                              (uVar4,&Crops__TPlayerCropStructure_9123EEE6);
            cVar1 = iVar5 == 0;
          }
          iVar6 = iVar6 + 1;
          HSEngine_CenterHNtoHX_A52F39E4(local_14,local_18,iVar6,&local_20,&local_1c);
        }
      }
    }
  }
  return cVar1;
}

