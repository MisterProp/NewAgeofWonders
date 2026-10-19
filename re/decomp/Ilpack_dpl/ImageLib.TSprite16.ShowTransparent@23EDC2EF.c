// /Ilpack.dpl : ImageLib.TSprite16.ShowTransparent@23EDC2EF

void ImageLib_TSprite16_ShowTransparent_23EDC2EF
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
                    /* 0x11ea0  542  ImageLib.TSprite16.ShowTransparent@23EDC2EF */
  uVar1 = GFXE_TLookupBlendTableCollection100_GetLookupTables_23EDC2EF
                    (*(undefined4 *)GFXE_AlphaBlendTable_AB1D79F5_exref,0x32,param_3,param_4);
  (**(code **)(*param_1 + 0x94))(param_1,param_2,param_3,uVar1);
  return;
}

