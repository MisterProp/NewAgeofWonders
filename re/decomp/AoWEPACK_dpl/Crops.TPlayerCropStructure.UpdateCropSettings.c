
void Crops_TPlayerCropStructure_UpdateCropSettings_23EDC2EF(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int local_14;
  int local_10;
  
                    /* 0xa636c  6732  Crops.TPlayerCropStructure.UpdateCropSettings@23EDC2EF */
  iVar1 = *(int *)(*(int *)(param_1 + 4) + 4);
  iVar4 = *(int *)(*(int *)(*(int *)(param_1 + 8) + 0x74) + 8) + -1;
  if (-1 < iVar4) {
    do {
      HSEngine_CenterHNtoHX_A52F39E4
                ((int)*(char *)(param_1 + 0x10),(int)*(char *)(param_1 + 0x11),
                 *(undefined2 *)(*(int *)(*(int *)(*(int *)(param_1 + 8) + 0x74) + 4) + iVar4 * 2),
                 &local_10,&local_14);
      if ((((-1 < local_14) && (local_14 < *(int *)(iVar1 + 0xc))) && (-1 < local_10)) &&
         (local_10 < *(int *)(iVar1 + 0x10))) {
        piVar2 = (int *)HSEngine_TMapLevel_GetField_23EDC2EF(iVar1,local_14,local_10);
        iVar3 = (**(code **)(*piVar2 + 0x80))(piVar2,0x2036a);
        if (iVar3 != 0) {
          Crops_TCrop_UpdateSettings_23EDC2EF();
        }
      }
      iVar4 = iVar4 + -1;
    } while (iVar4 != -1);
  }
  return;
}

