// /Ilpack.dpl : ImageLib.TSprite16.ReadDiscardableImageData@23EDC2EF

void ImageLib_TSprite16_ReadDiscardableImageData_23EDC2EF(int param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  
                    /* 0x117ec  548  ImageLib.TSprite16.ReadDiscardableImageData@23EDC2EF */
  ImageLib_TImage16_ReadDiscardableImageData_23EDC2EF(param_1,param_2);
  ImageLib_TSprite16_AllocateDiscardableImageData_23EDC2EF
            (param_1,*(undefined4 *)(param_1 + 100),*(undefined4 *)(param_1 + 0x68));
  iVar1 = *(int *)(param_1 + 0x68);
  if (-1 < iVar1 + -1) {
    iVar2 = 0;
    do {
      (**(code **)(*param_2 + 4))
                (param_2,iVar2 * *(int *)(param_1 + 0x48) + *(int *)(param_1 + 0x50),
                 *(int *)(param_1 + 100) * 2);
      iVar2 = iVar2 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
  }
  *(undefined4 *)(param_1 + 0x58) = *(undefined4 *)(param_1 + 0x54);
  return;
}

