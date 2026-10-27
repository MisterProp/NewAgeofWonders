// /aowInt.dpl : AOWInterfaceImgLib.TInterfaceIL.DrawILLh@23EDC2EF

void AOWInterfaceImgLib_TInterfaceIL_DrawILLh_23EDC2EF
               (undefined4 param_1,int param_2,int *param_3,int param_4)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  int *piVar6;
  undefined4 *puVar7;
  int *piVar8;
  byte bVar9;
  undefined4 local_34 [4];
  int local_24 [4];
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
                    /* 0x4028  53  AOWInterfaceImgLib.TInterfaceIL.DrawILLh@23EDC2EF */
  bVar9 = 0;
  piVar6 = (int *)(param_4 + 0x10);
  piVar8 = local_24;
  for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar8 = *piVar6;
    piVar6 = piVar6 + 1;
    piVar8 = piVar8 + 1;
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
  puVar5 = local_34;
  puVar7 = (undefined4 *)(param_4 + 0x10);
  for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar7 = *puVar5;
    puVar5 = puVar5 + (uint)bVar9 * -2 + 1;
    puVar7 = puVar7 + (uint)bVar9 * -2 + 1;
  }
  uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2,0,param_4);
  ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,*param_3,param_3[1]);
  iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2);
  if ((*param_3 + param_3[2]) - *(int *)(iVar3 + 0x20) < local_24[2]) {
    iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2);
    *(int *)(param_4 + 0x18) = (*param_3 + param_3[2]) - *(int *)(iVar3 + 0x20);
  }
  iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
  iVar4 = param_3[2] - *(int *)(iVar3 + 0x20);
  iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2);
  iVar4 = iVar4 - *(int *)(iVar3 + 0x20);
  iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 1);
  iVar4 = iVar4 / *(int *)(iVar3 + 0x20);
  if (-1 < iVar4) {
    local_14 = iVar4 + 1;
    local_8 = 0;
    do {
      iVar3 = param_4;
      iVar4 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2);
      iVar4 = *(int *)(iVar4 + 0x20) + *param_3;
      iVar2 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 1);
      iVar4 = iVar4 + *(int *)(iVar2 + 0x20) * local_8;
      uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 1);
      ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,iVar4,param_3[1],iVar3);
      local_8 = local_8 + 1;
      local_14 = local_14 + -1;
    } while (local_14 != 0);
  }
  Classes_Rect_9923E97E
            (*(undefined4 *)(param_4 + 0x10),*(undefined4 *)(param_4 + 0x14),local_c,local_34,
             local_10);
  puVar5 = local_34;
  puVar7 = (undefined4 *)(param_4 + 0x10);
  for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar7 = *puVar5;
    puVar5 = puVar5 + (uint)bVar9 * -2 + 1;
    puVar7 = puVar7 + (uint)bVar9 * -2 + 1;
  }
  iVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2,0,param_4);
  iVar3 = (*param_3 + param_3[2]) - *(int *)(iVar3 + 0x20);
  uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(param_1,param_2 + 2);
  ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,iVar3,param_3[1]);
  piVar6 = local_24;
  piVar8 = (int *)(param_4 + 0x10);
  for (iVar3 = 4; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar8 = *piVar6;
    piVar6 = piVar6 + (uint)bVar9 * -2 + 1;
    piVar8 = piVar8 + (uint)bVar9 * -2 + 1;
  }
  return;
}

