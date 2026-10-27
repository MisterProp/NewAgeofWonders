// /Ilpack.dpl : ImageLib.TShadow.SupportShowMode@23EDC2EF

undefined4
ImageLib_TShadow_SupportShowMode_23EDC2EF(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  
                    /* 0x12a04  516  ImageLib.TShadow.SupportShowMode@23EDC2EF */
  if (((param_3 == 0x56509310) || (param_3 == 0x55509310)) &&
     (cVar1 = ImageLib_TLibraryImage_SupportShowMode_23EDC2EF(param_1,param_2,param_3),
     cVar1 != '\0')) {
    return 1;
  }
  return 0;
}

