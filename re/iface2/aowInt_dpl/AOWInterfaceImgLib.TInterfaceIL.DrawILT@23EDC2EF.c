// /aowInt.dpl : AOWInterfaceImgLib.TInterfaceIL.DrawILT@23EDC2EF

void AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF
               (undefined4 param_1,undefined4 param_2,int *param_3,int param_4)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *puVar5;
  int *piVar6;
  undefined4 *puVar7;
  int *piVar8;
  byte bVar9;
  int iVar10;
  undefined4 local_3c [4];
  int local_2c [4];
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
                    /* 0x3e24  55  AOWInterfaceImgLib.TInterfaceIL.DrawILT@23EDC2EF */
  bVar9 = 0;
  piVar6 = (int *)(param_4 + 0x10);
  piVar8 = local_2c;
  for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
    *piVar8 = *piVar6;
    piVar6 = piVar6 + 1;
    piVar8 = piVar8 + 1;
  }
  local_10 = *(int *)(param_4 + 0x18);
  if (*param_3 + param_3[2] <= local_10) {
    local_10 = *param_3 + param_3[2];
  }
  local_14 = *(int *)(param_4 + 0x1c);
  if (param_3[1] + param_3[3] <= local_14) {
    local_14 = param_3[1] + param_3[3];
  }
  Classes_Rect_9923E97E
            (*(undefined4 *)(param_4 + 0x10),*(undefined4 *)(param_4 + 0x14),local_10,local_3c,
             local_14);
  puVar5 = local_3c;
  puVar7 = (undefined4 *)(param_4 + 0x10);
  for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar7 = *puVar5;
    puVar5 = puVar5 + (uint)bVar9 * -2 + 1;
    puVar7 = puVar7 + (uint)bVar9 * -2 + 1;
  }
  if (*param_3 + param_3[2] < local_2c[2]) {
    *(int *)(param_4 + 0x18) = *param_3 + param_3[2];
  }
  if (param_3[1] + param_3[3] < local_2c[3]) {
    *(int *)(param_4 + 0x1c) = param_3[1] + param_3[3];
  }
  iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
  local_18 = param_3[3] / *(int *)(iVar4 + 0x24);
  if (-1 < local_18) {
    local_18 = local_18 + 1;
    local_c = 0;
    do {
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
      iVar4 = param_3[2] / *(int *)(iVar4 + 0x20);
      if (-1 < iVar4) {
        local_1c = iVar4 + 1;
        local_8 = 0;
        do {
          iVar10 = param_4;
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
          iVar1 = *(int *)(iVar4 + 0x24) * local_c + param_3[1];
          iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
          iVar2 = *(int *)(iVar4 + 0x20) * local_8;
          iVar4 = *param_3;
          uVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
          ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar3,iVar2 + iVar4,iVar1,iVar10);
          local_8 = local_8 + 1;
          local_1c = local_1c + -1;
        } while (local_1c != 0);
      }
      local_c = local_c + 1;
      local_18 = local_18 + -1;
    } while (local_18 != 0);
  }
  piVar6 = local_2c;
  piVar8 = (int *)(param_4 + 0x10);
  for (iVar4 = 4; iVar4 != 0; iVar4 = iVar4 + -1) {
    *piVar8 = *piVar6;
    piVar6 = piVar6 + (uint)bVar9 * -2 + 1;
    piVar8 = piVar8 + (uint)bVar9 * -2 + 1;
  }
  return;
}

