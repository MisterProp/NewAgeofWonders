// /Ilpack.dpl : ImageLib.TImage16.ReadImageData@23EDC2EF

void ImageLib_TImage16_ReadImageData_23EDC2EF(int param_1,int *param_2)

{
                    /* 0xe8b0  613  ImageLib.TImage16.ReadImageData@23EDC2EF */
  ImageLib_TLibraryImage_ReadImageData_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x44,4);
  *(undefined4 *)(param_1 + 0x40) = *(undefined4 *)(param_1 + 0x44);
  return;
}

