// /Ilpack.dpl : ImageLib.TLibraryImage.ShowEx@23EDC2EF

void ImageLib_TLibraryImage_ShowEx_23EDC2EF
               (int *param_1,undefined4 param_2,undefined4 param_3,char *param_4,undefined4 param_5)

{
  char cVar1;
  
                    /* 0xe5c8  624  ImageLib.TLibraryImage.ShowEx@23EDC2EF */
  cVar1 = *param_4;
  if (cVar1 == '\0') {
    (**(code **)(*param_1 + 0x8c))(param_1,param_2,param_3,param_5);
  }
  else if (cVar1 == '\x01') {
    (**(code **)(*param_1 + 0x9c))(param_1,param_2,param_3,param_5);
  }
  else if (cVar1 == '\x02') {
    switch(*(undefined1 *)((int)param_1 + 0x39)) {
    case 0:
      if (*(int *)(param_4 + 4) != 0) {
        (**(code **)(*param_1 + 0x94))(param_1,param_2,param_3,*(undefined4 *)(param_4 + 4),param_5)
        ;
      }
      break;
    case 1:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)
                  (*(int *)GFXE_AlphaBlendTable_AB1D79F5_exref + 8 + *(int *)(param_4 + 4) * 4),
                 param_5);
      break;
    case 2:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)
                  (*(int *)GFXE_BrightenBlendTable_AB1D79F5_exref + 8 + *(int *)(param_4 + 4) * 4),
                 param_5);
      break;
    case 3:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)
                  (*(int *)GFXE_IntensityBlendTable_AB1D79F5_exref + 8 + *(int *)(param_4 + 4) * 4),
                 param_5);
      break;
    case 4:
      (**(code **)(*param_1 + 0x94))
                (param_1,param_2,param_3,
                 *(undefined4 *)
                  (*(int *)GFXE_ShadowBlendTable_AB1D79F5_exref + 8 + *(int *)(param_4 + 4) * 4),
                 param_5);
    }
  }
  return;
}

