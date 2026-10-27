// /Ilpack.dpl : ImageLib.BltLibraryImage@1A2A4D4B

void ImageLib_BltLibraryImage_1A2A4D4B
               (int param_1,int *param_2,LONG *param_3,LONG *param_4,char param_5,undefined4 param_6
               ,undefined4 param_7,int param_8,int param_9,char param_10)

{
  HDC pHVar1;
  uint uVar2;
  HRGN pHVar3;
  HBRUSH pHVar4;
  undefined4 uVar5;
  int iVar6;
  RECT *pRVar7;
  byte bVar8;
  float10 fVar9;
  float10 fVar10;
  LONG LVar11;
  LONG LVar12;
  HRGN pHVar13;
  LONG right;
  LONG bottom;
  undefined4 local_68;
  undefined4 local_64;
  int local_60;
  int local_5c;
  RECT local_58;
  RECT local_48;
  RECT local_38;
  HRGN local_28;
  undefined4 local_24;
  undefined1 local_20 [6];
  undefined1 local_1a [6];
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
                    /* 0x13230  512  ImageLib.BltLibraryImage@1A2A4D4B */
  bVar8 = 0;
  pRVar7 = &local_38;
  for (iVar6 = 4; iVar6 != 0; iVar6 = iVar6 + -1) {
    pRVar7->left = *param_3;
    param_3 = param_3 + 1;
    pRVar7 = (RECT *)&pRVar7->top;
  }
  if (((param_2 == (int *)0x0) || (param_2[8] == 0)) || (param_2[9] == 0)) {
    local_24 = Graphics_TBrush_GetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14));
    Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),param_6);
    pHVar4 = (HBRUSH)Graphics_TBrush_GetHandle_23EDC2EF(*(undefined4 *)(param_1 + 0x14));
    pRVar7 = &local_38;
    pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
    FillRect(pHVar1,pRVar7,pHVar4);
    Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),local_24);
  }
  else {
    iVar6 = param_2[8];
    local_5c = param_2[9];
    local_14 = local_5c;
    local_68 = 0;
    local_64 = 0;
    local_60 = iVar6;
    if (param_10 == '\0') {
      Graphics_TCanvas_GetClipRect_23EDC2EF(param_1,&local_58);
      LVar11 = local_38.left;
      LVar12 = local_38.top;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      IntersectClipRect(pHVar1,LVar11,LVar12,local_38.right,local_38.bottom);
      local_24 = Graphics_TBrush_GetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14));
      Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),param_7);
      local_8 = (param_2[9] * param_8) / 100;
      local_48.left = local_38.left;
      local_48.top = local_38.top;
      local_48.right = (param_2[8] * param_9) / 100 + local_38.left;
      local_48.bottom = local_38.top + local_8;
      local_28 = CreateRectRgnIndirect(&local_48);
      pHVar3 = CreateRectRgnIndirect(&local_38);
      CombineRgn(pHVar3,pHVar3,local_28,4);
      pHVar4 = (HBRUSH)Graphics_TBrush_GetHandle_23EDC2EF(*(undefined4 *)(param_1 + 0x14));
      pHVar13 = pHVar3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      FillRgn(pHVar1,pHVar13,pHVar4);
      DeleteObject(local_28);
      DeleteObject(pHVar3);
      iVar6 = 3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      SetStretchBltMode(pHVar1,iVar6);
      uVar5 = Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      (**(code **)(*param_2 + 0xa4))(param_2,&local_68,&local_48,param_7,uVar5);
      Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),local_24);
      pHVar3 = CreateRectRgnIndirect(&local_58);
      pHVar13 = pHVar3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      SelectObject(pHVar1,pHVar13);
      DeleteObject(pHVar3);
    }
    else if (param_10 == '\x01') {
      iVar6 = 3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      SetStretchBltMode(pHVar1,iVar6);
      uVar5 = Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      (**(code **)(*param_2 + 0xa4))(param_2,&local_68,&local_38,param_7,uVar5);
    }
    else if (param_10 == '\x02') {
      local_24 = Graphics_TBrush_GetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14));
      if (param_5 == '\0') {
        Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),param_7);
      }
      else {
        Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),param_6);
      }
      local_c = local_38.right - local_38.left;
      local_10 = local_38.bottom - local_38.top;
      local_8 = local_14;
      fVar9 = (float10)local_c / (float10)iVar6;
      System__Ext2Real_51F89FF7(local_1a);
      fVar10 = (float10)local_10 / (float10)local_8;
      System__Ext2Real_51F89FF7(local_20);
      System__Real2Ext_51F89FF7(local_1a);
      System__Real2Ext_51F89FF7(local_20);
      if (fVar9 <= fVar10) {
        System__Real2Ext_51F89FF7(local_20);
        System__Ext2Real_51F89FF7(local_1a);
      }
      else {
        System__Real2Ext_51F89FF7(local_1a);
        System__Ext2Real_51F89FF7(local_20);
      }
      System__Real2Ext_51F89FF7(local_1a);
      iVar6 = System__ROUND_51F89FF7();
      System__Real2Ext_51F89FF7(local_20);
      local_8 = System__ROUND_51F89FF7();
      local_48.left = local_c - iVar6 >> 1;
      if (local_48.left < 0) {
        local_48.left = local_48.left + (uint)((local_c - iVar6 & 1U) != 0);
      }
      local_48.left = local_48.left + local_38.left;
      local_48.top = local_10 - local_8 >> 1;
      if (local_48.top < 0) {
        local_48.top = local_48.top + (uint)((local_10 - local_8 & 1U) != 0);
      }
      local_48.top = local_48.top + local_38.top;
      local_48.right = iVar6 + local_48.left;
      local_48.bottom = local_48.top + local_8;
      local_28 = CreateRectRgnIndirect(&local_48);
      pHVar3 = CreateRectRgnIndirect(&local_38);
      CombineRgn(pHVar3,pHVar3,local_28,4);
      pHVar4 = (HBRUSH)Graphics_TBrush_GetHandle_23EDC2EF(*(undefined4 *)(param_1 + 0x14));
      pHVar13 = pHVar3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      FillRgn(pHVar1,pHVar13,pHVar4);
      DeleteObject(local_28);
      DeleteObject(pHVar3);
      iVar6 = 3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      SetStretchBltMode(pHVar1,iVar6);
      uVar5 = Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      (**(code **)(*param_2 + 0xa4))(param_2,&local_68,&local_48,param_7,uVar5);
      Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),local_24);
    }
    else if (param_10 == '\x03') {
      Graphics_TCanvas_GetClipRect_23EDC2EF(param_1,&local_58);
      LVar11 = local_38.left;
      LVar12 = local_38.top;
      right = local_38.right;
      bottom = local_38.bottom;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      IntersectClipRect(pHVar1,LVar11,LVar12,right,bottom);
      local_24 = Graphics_TBrush_GetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14));
      if (param_5 == '\0') {
        Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),param_7);
      }
      else {
        Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),param_6);
      }
      local_c = local_38.right - local_38.left;
      local_10 = local_38.bottom - local_38.top;
      local_48.right = (iVar6 * param_9) / 100;
      local_8 = (local_14 * param_8) / 100;
      uVar2 = local_c - local_48.right;
      local_48.left = (int)uVar2 >> 1;
      if (local_48.left < 0) {
        local_48.left = local_48.left + (uint)((uVar2 & 1) != 0);
      }
      local_48.left = local_48.left + local_38.left;
      local_48.top = local_10 - local_8 >> 1;
      if (local_48.top < 0) {
        local_48.top = local_48.top + (uint)((local_10 - local_8 & 1U) != 0);
      }
      local_48.top = local_48.top + local_38.top;
      local_48.right = local_48.right + local_48.left;
      local_48.bottom = local_48.top + local_8;
      local_28 = CreateRectRgnIndirect(&local_48);
      pHVar3 = CreateRectRgnIndirect(&local_38);
      CombineRgn(pHVar3,pHVar3,local_28,4);
      pHVar4 = (HBRUSH)Graphics_TBrush_GetHandle_23EDC2EF(*(undefined4 *)(param_1 + 0x14));
      pHVar13 = pHVar3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      FillRgn(pHVar1,pHVar13,pHVar4);
      DeleteObject(local_28);
      DeleteObject(pHVar3);
      iVar6 = 3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      SetStretchBltMode(pHVar1,iVar6);
      uVar5 = Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      (**(code **)(*param_2 + 0xa4))(param_2,&local_68,&local_48,param_7,uVar5);
      Graphics_TBrush_SetColor_23EDC2EF(*(undefined4 *)(param_1 + 0x14),local_24);
      pHVar3 = CreateRectRgnIndirect(&local_58);
      pHVar13 = pHVar3;
      pHVar1 = (HDC)Graphics_TCanvas_GetHandle_23EDC2EF(param_1);
      SelectObject(pHVar1,pHVar13);
      DeleteObject(pHVar3);
      if (param_5 != '\0') {
        if (local_38.left < local_48.left) {
          local_38.left = local_48.left;
        }
        if (local_48.right < local_38.right) {
          local_38.right = local_48.right;
        }
        if (local_38.top < local_48.top) {
          local_38.top = local_48.top;
        }
        if (local_48.bottom < local_38.bottom) {
          local_38.bottom = local_48.bottom;
        }
      }
    }
  }
  pRVar7 = &local_38;
  for (iVar6 = 4; iVar6 != 0; iVar6 = iVar6 + -1) {
    *param_4 = pRVar7->left;
    pRVar7 = (RECT *)((int)pRVar7 + ((uint)bVar8 * -2 + 1) * 4);
    param_4 = param_4 + (uint)bVar8 * -2 + 1;
  }
  return;
}

