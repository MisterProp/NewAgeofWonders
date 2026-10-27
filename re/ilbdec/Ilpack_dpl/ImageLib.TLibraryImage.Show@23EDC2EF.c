// /Ilpack.dpl : ImageLib.TLibraryImage.Show@23EDC2EF

void ImageLib_TLibraryImage_Show_23EDC2EF
               (int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  
                    /* 0xe468  625  ImageLib.TLibraryImage.Show@23EDC2EF */
  cVar1 = (char)param_1[0xe];
  if (cVar1 == '\0') {
    (**(code **)(*param_1 + 0x8c))(param_1,param_2,param_3,param_4);
  }
  else if (cVar1 == '\x01') {
    (**(code **)(*param_1 + 0x9c))(param_1,param_2,param_3,param_4);
  }
  else if (cVar1 == '\x02') {
    switch(*(undefined1 *)((int)param_1 + 0x39)) {
    case 0:
      if (param_1[0xf] != 0) {
        (**(code **)(*param_1 + 0x94))(param_1,param_2,param_3,param_1[0xf],param_4);
      }
      break;
    case 1:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)(*(int *)GFXE_AlphaBlendTable_AB1D79F5_exref + 8 + param_1[0xf] * 4)
                 ,param_4);
      break;
    case 2:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)
                  (*(int *)GFXE_BrightenBlendTable_AB1D79F5_exref + 8 + param_1[0xf] * 4),param_4);
      break;
    case 3:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)
                  (*(int *)GFXE_IntensityBlendTable_AB1D79F5_exref + 8 + param_1[0xf] * 4),param_4);
      break;
    case 4:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)
                  (*(int *)GFXE_ShadowBlendTable_AB1D79F5_exref + 8 + param_1[0xf] * 4),param_4);
      break;
    case 5:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)
                  (*(int *)GFXE_LinearAlphaBlendTable_AB1D79F5_exref + 8 + param_1[0xf] * 4),param_4
                );
    }
  }
  return;
}

