// /Ilpack.dpl : ImageLib.TLibraryImage.ReadImageData@23EDC2EF

void ImageLib_TLibraryImage_ReadImageData_23EDC2EF(int param_1,int *param_2)

{
                    /* 0xdbac  649  ImageLib.TLibraryImage.ReadImageData@23EDC2EF */
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x15,1);
  FUN_5520d1e0(param_1 + 0x30,param_2);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x18,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x1c,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x28,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x2c,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x34,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x16,1);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x10,4);
  if (*(byte *)(param_1 + 0x15) == 0) {
    *(int *)(param_1 + 0x20) = *(int *)(param_1 + 0x18) + *(int *)(param_1 + 0x28);
    *(int *)(param_1 + 0x24) = *(int *)(param_1 + 0x1c) + *(int *)(param_1 + 0x2c);
  }
  else {
    if (1 < *(byte *)(param_1 + 0x15)) {
      (**(code **)(*param_2 + 4))(param_2,param_1 + 0xc,4);
    }
    (**(code **)(*param_2 + 4))(param_2,param_1 + 0x20,4);
    (**(code **)(*param_2 + 4))(param_2,param_1 + 0x24,4);
  }
  if (2 < *(byte *)(param_1 + 0x15)) {
    (**(code **)(*param_2 + 4))(param_2,param_1 + 0x38,8);
  }
  return;
}

