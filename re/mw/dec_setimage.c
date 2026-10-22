void ImageLib_TCustomImageLibrary_SetImage_23EDC2EF(int *param_1,int param_2,int *param_3)
{
  int *piVar1;
  int iVar2;
                    /* 0x1b490  250  ImageLib.TCustomImageLibrary.SetImage@23EDC2EF */
  if (param_1[0x11] <= param_2) {
    ImageLib_TCustomImageLibrary_SetCount_23EDC2EF(param_1,param_2 + 1);
  }
  piVar1 = *(int **)(param_1[0x12] + param_2 * 8);
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + -4))(piVar1,1);
  }
  iVar2 = param_1[0x12];
  *(int **)(iVar2 + param_2 * 8) = param_3;
  *(undefined4 *)(iVar2 + 4 + param_2 * 8) = 0xffffffff;
  ImageLib_TCustomImageLibrary_UpdateImageItemSettings_23EDC2EF(param_1,param_2);
  if (param_3 != (int *)0x0) {
    (**(code **)(*param_3 + 0xc))(param_3,param_1);
  }
  (**(code **)(*param_1 + 0x2c))(param_1,1);
  if ((char)param_1[0xc] == '\0') {
    (**(code **)(*param_1 + 0x44))();
  }
  return;
}
