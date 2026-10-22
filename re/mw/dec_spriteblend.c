void ImageLib_TSprite16_ShowBlended_23EDC2EF
               (int *param_1,int param_2,int param_3,undefined4 param_4,int param_5)
{
  int iVar1;
  int iVar2;
  int local_14;
  int local_10;
                    /* 0x11b64  543  ImageLib.TSprite16.ShowBlended@23EDC2EF */
  iVar2 = param_1[10] + param_2 + param_1[0x17];
  local_10 = param_1[0x19];
  iVar1 = local_10 + iVar2;
  if (iVar2 < *(int *)(param_5 + 0x10)) {
    local_10 = local_10 - (*(int *)(param_5 + 0x10) - (param_2 + param_1[0x17]));
  }
  if (*(int *)(param_5 + 0x18) < iVar1) {
    local_10 = local_10 - (iVar1 - *(int *)(param_5 + 0x18));
  }
  if (0 < local_10) {
    iVar1 = param_3 + param_1[0x18] + param_1[0xb];
    local_14 = param_1[0x1a];
    iVar2 = iVar1 + local_14;
    if (iVar1 < *(int *)(param_5 + 0x14)) {
      local_14 = local_14 - (*(int *)(param_5 + 0x14) - (param_3 + param_1[0x18]));
    }
    if (*(int *)(param_5 + 0x1c) < iVar2) {
      local_14 = local_14 - (iVar2 - *(int *)(param_5 + 0x1c));
    }
    if (0 < local_14) {
      if ((char)param_1[2] == '\0') {
        (**(code **)(*param_1 + 0x50))();
        iVar1 = param_1[0x10];
        if (*(int *)(param_5 + 0x28) == iVar1) {
          if (iVar1 == 0x55509310) {
            GFXE_BlendColorKey555_981312BC();
          }
          else if (iVar1 == 0x56509310) {
            GFXE_BlendColorKey565_981312BC();
          }
        }
        else {
          iVar1 = (**(code **)(*param_1 + 0x5c))(param_1,*(undefined4 *)(param_5 + 0x28));
          if (iVar1 != 0) {
            SysUtils_Exception_Create_23EDC2EF
                      (SysUtils__Exception_9880D0BF_exref,1,"Surface pixel format not supported!");
            System__RaiseExcept_51F89FF7();
          }
          if (param_1[0x10] == 0x55509310) {
            GFXE_BlendColorKey555_981312BC();
          }
          else if (param_1[0x10] == 0x56509310) {
            GFXE_BlendColorKey565_981312BC();
          }
        }
        if (*(char *)((int)param_1 + 0x16) == '\x02') {
          (**(code **)(*param_1 + 0x4c))();
        }
      }
      else if (*(int *)(param_5 + 0x28) == param_1[0x10]) {
        if (param_1[0x10] == 0x55509310) {
          GFXE_BlendColorKey555_981312BC();
        }
        else if (param_1[0x10] == 0x56509310) {
          GFXE_BlendColorKey565_981312BC();
        }
      }
      else {
        iVar1 = (**(code **)(*param_1 + 0x5c))(param_1,*(undefined4 *)(param_5 + 0x28));
        if (iVar1 != 0) {
          SysUtils_Exception_Create_23EDC2EF
                    (SysUtils__Exception_9880D0BF_exref,1,"Surface pixel format not supported!");
          System__RaiseExcept_51F89FF7();
        }
        if (param_1[0x10] == 0x55509310) {
          GFXE_BlendColorKey555_981312BC();
        }
        else if (param_1[0x10] == 0x56509310) {
          GFXE_BlendColorKey565_981312BC();
        }
      }
    }
  }
  return;
}
