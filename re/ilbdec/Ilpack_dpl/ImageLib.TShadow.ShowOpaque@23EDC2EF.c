// /Ilpack.dpl : ImageLib.TShadow.ShowOpaque@23EDC2EF

void ImageLib_TShadow_ShowOpaque_23EDC2EF
               (int *param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
                    /* 0x12a64  513  ImageLib.TShadow.ShowOpaque@23EDC2EF */
  if ((char)param_1[2] == '\0') {
    (**(code **)(*param_1 + 0x50))();
    if (*(int *)(param_4 + 0x28) == 0x55509310) {
      FUN_55212eec();
    }
    else if (*(int *)(param_4 + 0x28) == 0x56509310) {
      FUN_55212ba8();
    }
    else {
      SysUtils_Exception_Create_23EDC2EF
                (SysUtils__Exception_9880D0BF_exref,1,"Surface format not supported");
      System__RaiseExcept_51F89FF7();
    }
    if (*(char *)((int)param_1 + 0x16) == '\x02') {
      (**(code **)(*param_1 + 0x4c))();
    }
  }
  else if (*(int *)(param_4 + 0x28) == 0x55509310) {
    FUN_55212eec();
  }
  else if (*(int *)(param_4 + 0x28) == 0x56509310) {
    FUN_55212ba8();
  }
  else {
    SysUtils_Exception_Create_23EDC2EF
              (SysUtils__Exception_9880D0BF_exref,1,"Surface format not supported");
    System__RaiseExcept_51F89FF7();
  }
  return;
}

