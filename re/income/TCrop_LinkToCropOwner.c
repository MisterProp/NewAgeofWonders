

void Crops_TCrop_LinkToCropOwner_23EDC2EF(int param_1)



{

  int iVar1;

  undefined4 uVar2;

  int iVar3;

  undefined1 local_30 [12];

  int local_24;

  int local_20;

  int local_1c;

  

                    /* 0xa5dac  6758  Crops.TCrop.LinkToCropOwner@23EDC2EF */

  iVar3 = 0;

  iVar1 = *(int *)(*(int *)(param_1 + 4) + 4);

  HSEngine_InitHNWalk_3DC7E9B4

            (local_30,(int)*(char *)(param_1 + 0x10),(int)*(char *)(param_1 + 0x11),1);

  while ((local_24 < 2 && (iVar3 == 0))) {

    if ((-1 < local_20) &&

       (((-1 < local_1c && (local_20 < *(int *)(iVar1 + 0xc))) &&

        (local_1c < *(int *)(iVar1 + 0x10))))) {

      uVar2 = HSEngine_TMapLevel_GetField_23EDC2EF(iVar1,local_20,local_1c);

      iVar3 = HSEngine_TMapField_FindNoneTransparentHS_23EDC2EF

                        (uVar2,&Crops__TPlayerCropStructure_9123EEE6);

    }

    HSEngine_WalkToNextHN_77BE26A7(local_30);

  }

  Crops_TCrop_SetCropOwner_23EDC2EF(param_1,iVar3);

  return;

}




