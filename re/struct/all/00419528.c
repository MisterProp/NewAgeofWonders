
void FUN_00419528(int param_1,uint *param_2)

{
  undefined1 *puVar1;
  int iVar2;
  uint uVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_40;
  undefined1 *puStack_3c;
  undefined1 *puStack_38;
  undefined4 local_2c;
  int local_28;
  int *local_24;
  undefined1 local_20;
  undefined1 local_1f;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint *local_8;
  
  puStack_38 = &stack0xfffffffc;
  local_2c = 0;
  puStack_3c = &LAB_00419775;
  uStack_40 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_40;
  *(int *)(param_1 + 0xa0) = *(int *)(param_1 + 0xa0) + 1;
  local_20 = 2;
  local_1f = 1;
  local_8 = param_2;
  local_24 = (int *)Classes_TList_Get_23EDC2EF
                              (*(undefined4 *)(param_1 + 0x98),*(undefined4 *)(param_1 + 0x9c));
  local_c = (int)*local_8 >> 1;
  if (local_c < 0) {
    local_c = local_c + (uint)((*local_8 & 1) != 0);
  }
  iVar2 = (**(code **)(*local_24 + 0x14))();
  uVar3 = iVar2 * 0x19 + *(int *)(*(int *)(param_1 + 0xb0) + 0x54);
  iVar2 = (int)uVar3 >> 1;
  if (iVar2 < 0) {
    iVar2 = iVar2 + (uint)((uVar3 & 1) != 0);
  }
  local_10 = (int)local_8[1] >> 1;
  if (local_10 < 0) {
    local_10 = local_10 + (uint)((local_8[1] & 1) != 0);
  }
  local_10 = local_10 - (iVar2 + 0x23);
  iVar2 = (**(code **)(*local_24 + 0x14))();
  local_18 = iVar2 * 0x1e + 0x46;
  iVar2 = (**(code **)(*local_24 + 0x14))();
  if (0 < iVar2) {
    local_1c = *(int *)(param_1 + 0xa0);
    if (local_18 < local_1c) {
      local_1c = (local_1c - local_18) * -5 + 100;
    }
    else {
      local_1c = local_1c * 10;
    }
    if (0 < local_1c) {
      if (local_1c < 100) {
        (**(code **)(*local_24 + 0xc))(local_24,0,&local_2c);
        ImageLib_TImageLibraryFont_SetTextEx_23EDC2EF
                  (*(undefined4 *)(param_1 + 0xb0),local_c,local_10,&local_20,1,local_8,local_2c);
      }
      else {
        (**(code **)(*local_24 + 0xc))(local_24,0,&local_2c);
        ImageLib_TImageLibraryFont_SetTextCentered_23EDC2EF
                  (*(undefined4 *)(param_1 + 0xb0),local_c,local_10,local_8,local_2c);
      }
    }
    iVar2 = (**(code **)(*local_24 + 0x14))();
    if (0 < iVar2 + -1) {
      local_14 = 1;
      local_28 = iVar2 + -1;
      do {
        iVar2 = *(int *)(param_1 + 0xa0);
        if (local_18 < iVar2) {
          iVar2 = (iVar2 - local_18) * -5 + 100;
        }
        else {
          iVar2 = (iVar2 - (local_14 * 0x1e + 0x14)) * 10;
        }
        if (0 < iVar2) {
          local_1c = iVar2;
          if (iVar2 < 100) {
            (**(code **)(*local_24 + 0xc))(local_24,local_14,&local_2c);
            ImageLib_TImageLibraryFont_SetTextEx_23EDC2EF
                      (*(undefined4 *)(param_1 + 0xb0),local_c,local_14 * 0x19 + local_10 + 5,
                       &local_20,1,local_8,local_2c);
          }
          else {
            (**(code **)(*local_24 + 0xc))(local_24,local_14,&local_2c);
            ImageLib_TImageLibraryFont_SetTextCentered_23EDC2EF
                      (*(undefined4 *)(param_1 + 0xb0),local_c,local_14 * 0x19 + local_10 + 5,
                       local_8,local_2c);
          }
        }
        local_14 = local_14 + 1;
        local_28 = local_28 + -1;
      } while (local_28 != 0);
    }
  }
  puVar1 = puStack_38;
  if (local_18 + 0x14 < *(int *)(param_1 + 0xa0)) {
    *(int *)(param_1 + 0x9c) = *(int *)(param_1 + 0x9c) + 1;
    if (*(int *)(param_1 + 0x9c) < *(int *)(*(int *)(param_1 + 0x98) + 8)) {
      *(undefined4 *)(param_1 + 0xa0) = 0;
    }
    else {
      *(undefined4 *)(param_1 + 0x68) = 0;
      *(undefined1 *)(param_1 + 0x95) = 0;
    }
  }
  *in_FS_OFFSET = uStack_40;
  puStack_38 = &LAB_0041977c;
  puStack_3c = (undefined1 *)0x419774;
  System__LStrClr_40929B27(&local_2c,uStack_40,puVar1);
  return;
}

