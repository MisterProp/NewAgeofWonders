// /Ilpack.dpl : ImageLib.TSprite16.ReadImageData@23EDC2EF

void ImageLib_TSprite16_ReadImageData_23EDC2EF(int param_1,int *param_2)

{
                    /* 0x1172c  550  ImageLib.TSprite16.ReadImageData@23EDC2EF */
  ImageLib_TImage16_ReadImageData_23EDC2EF(param_1,param_2);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 100,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x68,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x5c,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x60,4);
  (**(code **)(*param_2 + 4))(param_2,param_1 + 0x54,4);
  return;
}

