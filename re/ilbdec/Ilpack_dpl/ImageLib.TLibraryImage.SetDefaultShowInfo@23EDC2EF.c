// /Ilpack.dpl : ImageLib.TLibraryImage.SetDefaultShowInfo@23EDC2EF

void ImageLib_TLibraryImage_SetDefaultShowInfo_23EDC2EF(int *param_1,uint *param_2)

{
  uint uVar1;
  uint uVar2;
  char cVar3;
  
                    /* 0xe430  626  ImageLib.TLibraryImage.SetDefaultShowInfo@23EDC2EF */
  uVar1 = *param_2;
  uVar2 = param_2[1];
  cVar3 = (**(code **)(*param_1 + 0xa8))(param_1,uVar1 & 0xff,0);
  if (cVar3 != '\0') {
    param_1[0xe] = uVar1;
    param_1[0xf] = uVar2;
  }
  return;
}

