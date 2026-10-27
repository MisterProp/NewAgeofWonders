// /aowInt.dpl : AOWInterfaceImgLib.TInterfaceIL.DrawILF@23EDC2EF

void AOWInterfaceImgLib_TInterfaceIL_DrawILF_23EDC2EF
               (undefined4 param_1,int param_2,int *param_3,int param_4)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined4 *puVar9;
  int *piVar10;
  undefined4 *puVar11;
  int *piVar12;
  byte bVar13;
  undefined4 local_34 [4];
  int local_24 [4];
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
                    /* 0x3a5c  56  AOWInterfaceImgLib.TInterfaceIL.DrawILF@23EDC2EF */
  bVar13 = 0;
  piVar10 = (int *)(param_4 + 0x10);
  piVar12 = local_24;
  for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
    *piVar12 = *piVar10;
    piVar10 = piVar10 + 1;
    piVar12 = piVar12 + 1;
  }
  local_c = *(int *)(param_4 + 0x18);
  if (*param_3 + param_3[2] <= local_c) {
    local_c = *param_3 + param_3[2];
  }
  local_10 = *(int *)(param_4 + 0x1c);
  if (param_3[1] + param_3[3] <= local_10) {
    local_10 = param_3[1] + param_3[3];
  }
  Classes_Rect_9923E97E
            (*(undefined4 *)(param_4 + 0x10),*(undefined4 *)(param_4 + 0x14),local_c,local_34,
             local_10);
  puVar9 = local_34;
  puVar11 = (undefined4 *)(param_4 + 0x10);
  for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
    *puVar11 = *puVar9;
    puVar9 = puVar9 + (uint)bVar13 * -2 + 1;
    puVar11 = puVar11 + (uint)bVar13 * -2 + 1;
  }
  uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2,0,param_4);
  ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,*param_3,param_3[1]);
  iVar8 = param_4;
  iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 5);
  iVar2 = (param_3[1] + param_3[3]) - *(int *)(iVar2 + 0x24);
  uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 5);
  ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,*param_3,iVar2,iVar8);
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 4);
  if ((*param_3 + param_3[2]) - *(int *)(iVar8 + 0x20) < local_24[2]) {
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 4);
    *(int *)(param_4 + 0x18) = (*param_3 + param_3[2]) - *(int *)(iVar8 + 0x20);
  }
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
  iVar2 = param_3[2] - *(int *)(iVar8 + 0x20);
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2);
  iVar2 = iVar2 - *(int *)(iVar8 + 0x20);
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 1);
  iVar2 = iVar2 / *(int *)(iVar8 + 0x20);
  if (-1 < iVar2) {
    local_14 = iVar2 + 1;
    local_8 = 0;
    do {
      iVar8 = param_4;
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
      iVar2 = *(int *)(iVar2 + 0x20) + *param_3;
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 1);
      iVar2 = iVar2 + *(int *)(iVar3 + 0x20) * local_8;
      uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 1);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,iVar2,param_3[1],iVar8);
      iVar7 = param_4;
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 6);
      iVar8 = param_3[1];
      iVar2 = param_3[3];
      iVar3 = *(int *)(iVar3 + 0x24);
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 5);
      iVar4 = *(int *)(iVar4 + 0x20);
      iVar6 = *param_3;
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 6);
      iVar5 = *(int *)(iVar5 + 0x20) * local_8;
      uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 6);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF
                (uVar1,iVar4 + iVar6 + iVar5,(iVar8 + iVar2) - iVar3,iVar7);
      local_8 = local_8 + 1;
      local_14 = local_14 + -1;
    } while (local_14 != 0);
  }
  Classes_Rect_9923E97E
            (*(undefined4 *)(param_4 + 0x10),*(undefined4 *)(param_4 + 0x14),local_c,local_34,
             local_10);
  puVar9 = local_34;
  puVar11 = (undefined4 *)(param_4 + 0x10);
  for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
    *puVar11 = *puVar9;
    puVar9 = puVar9 + (uint)bVar13 * -2 + 1;
    puVar11 = puVar11 + (uint)bVar13 * -2 + 1;
  }
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 6);
  if ((param_3[1] + param_3[3]) - *(int *)(iVar8 + 0x24) < local_24[3]) {
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 6);
    *(int *)(param_4 + 0x1c) = (param_3[1] + param_3[3]) - *(int *)(iVar8 + 0x24);
  }
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
  iVar2 = param_3[3] - *(int *)(iVar8 + 0x24);
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 5);
  iVar2 = iVar2 - *(int *)(iVar8 + 0x24);
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 3);
  iVar2 = iVar2 / *(int *)(iVar8 + 0x24);
  if (-1 < iVar2) {
    local_14 = iVar2 + 1;
    local_8 = 0;
    do {
      iVar8 = param_4;
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
      iVar2 = *(int *)(iVar2 + 0x24) + param_3[1];
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 3);
      iVar2 = iVar2 + *(int *)(iVar3 + 0x24) * local_8;
      uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 3);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,*param_3,iVar2,iVar8);
      iVar7 = param_4;
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2);
      iVar8 = *(int *)(iVar8 + 0x24);
      iVar2 = param_3[1];
      iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 4);
      iVar5 = *(int *)(iVar3 + 0x24) * local_8;
      iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 4);
      iVar3 = *param_3;
      iVar4 = param_3[2];
      iVar6 = *(int *)(iVar6 + 0x20);
      uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 4);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF
                (uVar1,(iVar3 + iVar4) - iVar6,iVar8 + iVar2 + iVar5,iVar7);
      local_8 = local_8 + 1;
      local_14 = local_14 + -1;
    } while (local_14 != 0);
  }
  Classes_Rect_9923E97E
            (*(undefined4 *)(param_4 + 0x10),*(undefined4 *)(param_4 + 0x14),local_c,local_34,
             local_10);
  puVar9 = local_34;
  puVar11 = (undefined4 *)(param_4 + 0x10);
  for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
    *puVar11 = *puVar9;
    puVar9 = puVar9 + (uint)bVar13 * -2 + 1;
    puVar11 = puVar11 + (uint)bVar13 * -2 + 1;
  }
  iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2,0,param_4);
  iVar8 = (*param_3 + param_3[2]) - *(int *)(iVar8 + 0x20);
  uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2);
  ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,iVar8,param_3[1]);
  iVar5 = param_4;
  iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 7);
  iVar8 = param_3[1];
  iVar2 = param_3[3];
  iVar3 = *(int *)(iVar3 + 0x24);
  iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 7);
  iVar4 = *param_3;
  iVar6 = param_3[2];
  iVar7 = *(int *)(iVar7 + 0x20);
  uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 7);
  ImageLib_TLibraryImage_ShowClipped_23EDC2EF
            (uVar1,(iVar4 + iVar6) - iVar7,(iVar8 + iVar2) - iVar3,iVar5);
  piVar10 = local_24;
  piVar12 = (int *)(param_4 + 0x10);
  for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
    *piVar12 = *piVar10;
    piVar10 = piVar10 + (uint)bVar13 * -2 + 1;
    piVar12 = piVar12 + (uint)bVar13 * -2 + 1;
  }
  return;
}

