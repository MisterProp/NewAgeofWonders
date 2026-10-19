// /Ilpack.dpl : ImageLib.TImageLibraryFont.LoadFromStream@23EDC2EF

void ImageLib_TImageLibraryFont_LoadFromStream_23EDC2EF(int param_1)

{
  int iVar1;
  int iVar2;
  
                    /* 0x14254  504  ImageLib.TImageLibraryFont.LoadFromStream@23EDC2EF */
  ImageLib_TCustomImageLibrary_LoadFromStream_23EDC2EF(param_1);
  *(undefined4 *)(param_1 + 0x54) = 0;
  for (iVar2 = 0; iVar2 < *(int *)(param_1 + 0x44); iVar2 = iVar2 + 1) {
    iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,iVar2);
    if (iVar1 != 0) {
      iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,iVar2);
      if (*(int *)(param_1 + 0x54) < *(int *)(iVar1 + 0x24)) {
        iVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,iVar2);
        *(undefined4 *)(param_1 + 0x54) = *(undefined4 *)(iVar1 + 0x24);
      }
    }
  }
  return;
}

