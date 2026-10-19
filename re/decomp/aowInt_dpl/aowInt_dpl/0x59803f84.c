// /aowInt.dpl : 0x59803f84

void AOWInterfaceImgLib_TInterfaceIL_DrawILI_23EDC2EF
               (undefined4 param_1,undefined4 param_2,int *param_3,int param_4)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  byte bVar5;
  undefined4 local_30 [4];
  undefined4 local_20 [4];
  int local_10;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0x3f84  54  AOWInterfaceImgLib.TInterfaceIL.DrawILI@23EDC2EF */
  bVar5 = 0;
  puVar3 = (undefined4 *)(param_4 + 0x10);
  puVar4 = local_20;
  local_c = param_2;
  local_8 = param_1;
  for (iVar2 = 4; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  iVar2 = *param_3 + param_3[2];
  if (*(int *)(param_4 + 0x18) < iVar2) {
    iVar2 = *(int *)(param_4 + 0x18);
  }
  local_10 = *(int *)(param_4 + 0x1c);
  if (param_3[1] + param_3[3] <= local_10) {
    local_10 = param_3[1] + param_3[3];
  }
  Classes_Rect_9923E97E
            (*(undefined4 *)(param_4 + 0x10),*(undefined4 *)(param_4 + 0x14),iVar2,local_30,local_10
            );
  puVar3 = local_30;
  puVar4 = (undefined4 *)(param_4 + 0x10);
  for (iVar2 = 4; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + (uint)bVar5 * -2 + 1;
    puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
  }
  uVar1 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(local_8,local_c,0,param_4);
  ImageLib_TLibraryImage_ShowClipped_23EDC2EF(uVar1,*param_3,param_3[1]);
  puVar3 = local_20;
  puVar4 = (undefined4 *)(param_4 + 0x10);
  for (iVar2 = 4; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + (uint)bVar5 * -2 + 1;
    puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
  }
  return;
}

