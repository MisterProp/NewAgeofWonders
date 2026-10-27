// /Ilpack.dpl : ImageLib.TLibraryImage.LoadFromStream@23EDC2EF

void ImageLib_TLibraryImage_LoadFromStream_23EDC2EF(int *param_1,int *param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined2 extraout_var;
  
                    /* 0xde48  642  ImageLib.TLibraryImage.LoadFromStream@23EDC2EF */
  (**(code **)(*param_1 + 0x60))();
  (**(code **)(*param_1 + 0x3c))(param_1,param_2);
  if (*(byte *)((int)param_1 + 0x15) < 2) {
    iVar1 = Classes_TStream_GetPosition_23EDC2EF(param_2);
    param_1[3] = iVar1;
    if (((*(char *)((int)param_1 + 0x16) == '\0') || (param_1[1] == 0)) ||
       ((*(char *)(param_1[1] + 0x31) == '\0' || ((*(byte *)(param_1[1] + 0x24) & 1) == 0)))) {
      (**(code **)(*param_1 + 0x44))(param_1,param_2);
      *(undefined1 *)(param_1 + 5) = 0;
    }
    else {
      (**(code **)(*param_2 + 0xc))(param_2,param_1[4],CONCAT22(extraout_var,1));
      *(undefined1 *)(param_1 + 5) = 1;
    }
  }
  else if ((((*(char *)((int)param_1 + 0x16) == '\0') || (param_1[1] == 0)) ||
           (*(char *)(param_1[1] + 0x31) == '\0')) ||
          (((*(byte *)(param_1[1] + 0x24) & 1) == 0 || (param_1[3] == -1)))) {
    if (param_1[3] == -1) {
      (**(code **)(*param_1 + 0x44))(param_1,param_2);
    }
    else {
      uVar2 = Classes_TStream_GetPosition_23EDC2EF(param_2);
      Classes_TStream_SetPosition_23EDC2EF(param_2,*(int *)(param_1[1] + 0x3c) + param_1[3]);
      (**(code **)(*param_1 + 0x44))(param_1,param_2);
      Classes_TStream_SetPosition_23EDC2EF(param_2,uVar2);
    }
    *(undefined1 *)(param_1 + 5) = 0;
  }
  else {
    *(undefined1 *)(param_1 + 5) = 1;
  }
  return;
}

