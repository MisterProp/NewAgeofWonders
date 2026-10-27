// TAOWHScrollBar.Draw (/aowInt.dpl 0x5981ebc4)


void AoWHScroll_TAOWHScrollBar_Draw_23EDC2EF(int param_1,undefined4 *param_2)

{
  char cVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined1 *puVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  undefined4 *in_FS_OFFSET;
  undefined1 *puVar9;
  undefined4 uVar10;
  undefined4 uStack_7c;
  undefined1 *puStack_78;
  undefined1 *puStack_74;
  undefined4 uStack_70;
  undefined1 *puStack_6c;
  undefined1 *puStack_68;
  undefined4 local_58;
  undefined1 local_54 [16];
  undefined4 local_44 [15];
  int local_8;
  
                    /* 0x1ebc4  515  AoWHScroll.TAOWHScrollBar.Draw@23EDC2EF */
  puStack_68 = &stack0xfffffffc;
  puStack_74 = &stack0xfffffffc;
  local_58 = 0;
  puVar8 = local_44;
  for (iVar6 = 0xf; iVar6 != 0; iVar6 = iVar6 + -1) {
    *puVar8 = *param_2;
    param_2 = param_2 + 1;
    puVar8 = puVar8 + 1;
  }
  puStack_6c = &LAB_5981f496;
  uStack_70 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_70;
  puStack_78 = &LAB_5981f451;
  uStack_7c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_7c;
  puVar9 = &stack0xfffffffc;
  puVar4 = &stack0xfffffffc;
  local_8 = param_1;
  if ((*(char *)(param_1 + 0x2c) != '\0') &&
     (puVar9 = &stack0xfffffffc, puVar4 = &stack0xfffffffc, *(char *)(param_1 + 0x75) != '\0')) {
    puVar9 = &stack0xfffffffc;
    puVar4 = &stack0xfffffffc;
    if (*(short *)(param_1 + 0x3e) != 0) {
      (**(code **)(param_1 + 0x3c))(*(undefined4 *)(param_1 + 0x40),param_1,local_44);
      puVar9 = puStack_74;
      puVar4 = puStack_68;
    }
    puStack_68 = puVar4;
    puStack_74 = puVar9;
    cVar1 = *(char *)(local_8 + 0x160);
    if (cVar1 == '\0') {
      if ((*(char *)(local_8 + 0x151) == '\x04') && (*(char *)(local_8 + 0x150) != '\0')) {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x80);
        puVar9 = local_54;
        uVar2 = *(undefined4 *)(local_8 + 0x168);
        uVar3 = *(undefined4 *)(local_8 + 0x118);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar3,uVar2);
        iVar7 = (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84) +
                *(int *)(local_8 + 0x7c)) - *(int *)(iVar6 + 0x20);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(local_8 + 0x118),*(undefined4 *)(local_8 + 0x168),local_8
                           ,iVar7,puVar9,uVar10,puVar8);
        Classes_Rect_9923E97E
                  (iVar7,*(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(iVar6 + 0x20),puVar9,uVar10);
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF(uVar3,uVar2,local_54,puVar8);
      }
      else {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x80);
        puVar9 = local_54;
        uVar2 = *(undefined4 *)(local_8 + 0x118);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar2,*(undefined4 *)(local_8 + 0x168));
        iVar7 = (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84) +
                *(int *)(local_8 + 0x7c)) - *(int *)(iVar6 + 0x20);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(local_8 + 0x118),*(undefined4 *)(local_8 + 0x164),local_8
                           ,iVar7,puVar9,uVar10,puVar8);
        Classes_Rect_9923E97E
                  (iVar7,*(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(iVar6 + 0x20),puVar9,uVar10);
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF
                  (uVar2,*(undefined4 *)(local_8 + 0x164),local_54,puVar8);
      }
      if ((*(char *)(local_8 + 0x151) == '\x01') && (*(char *)(local_8 + 0x150) != '\0')) {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x80);
        puVar9 = local_54;
        uVar2 = *(undefined4 *)(local_8 + 0x170);
        uVar3 = *(undefined4 *)(local_8 + 0x11c);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar3,uVar2);
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(iVar6 + 0x20),puVar9,uVar10);
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF(uVar3,uVar2,local_54,puVar8);
      }
      else {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x80);
        puVar9 = local_54;
        uVar2 = *(undefined4 *)(local_8 + 0x16c);
        uVar3 = *(undefined4 *)(local_8 + 0x11c);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar3,uVar2);
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(iVar6 + 0x20),puVar9,uVar10);
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF(uVar3,uVar2,local_54,puVar8);
      }
      puVar8 = local_44;
      iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(local_8 + 0x120),*(undefined4 *)(local_8 + 0x174));
      uVar10 = *(undefined4 *)(iVar6 + 0x24);
      puVar9 = local_54;
      iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(local_8 + 0x11c),*(undefined4 *)(local_8 + 0x16c));
      Classes_Rect_9923E97E
                (*(int *)(iVar6 + 0x20) +
                 *(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                 *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                 *(undefined4 *)(local_8 + 0x130),puVar9,uVar10);
      AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF
                (*(undefined4 *)(local_8 + 0x120),*(undefined4 *)(local_8 + 0x174),local_54,puVar8);
      iVar6 = *(int *)(local_8 + 0x130);
      iVar7 = (iVar6 * *(int *)(local_8 + 0x148)) / *(int *)(local_8 + 0x14c);
      if (iVar6 < *(int *)(local_8 + 0x134) + iVar7) {
        iVar7 = iVar6 - *(int *)(local_8 + 0x134);
      }
      if (*(char *)(local_8 + 0x161) == '\0') {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x80);
        puVar9 = local_54;
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(local_8 + 0x11c),*(undefined4 *)(local_8 + 0x16c),iVar6,
                           puVar9,uVar10,puVar8);
        Classes_Rect_9923E97E
                  (*(int *)(iVar6 + 0x20) +
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84) + iVar7,
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(local_8 + 0x134),puVar9,uVar10);
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF
                  (*(undefined4 *)(local_8 + 0x128),*(undefined4 *)(local_8 + 0x178),local_54,puVar8
                  );
      }
      else if (*(char *)(local_8 + 0x161) == '\x01') {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x80);
        puVar9 = local_54;
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(local_8 + 0x11c),*(undefined4 *)(local_8 + 0x16c),iVar6,
                           puVar9,uVar10,puVar8);
        Classes_Rect_9923E97E
                  (*(int *)(iVar6 + 0x20) +
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84) + iVar7,
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(local_8 + 0x134),puVar9,uVar10);
        AOWInterfaceImgLib_TInterfaceIL_DrawILLh_23EDC2EF
                  (*(undefined4 *)(local_8 + 300),*(undefined4 *)(local_8 + 0x178),local_54,puVar8);
      }
    }
    else if (cVar1 == '\x01') {
      puVar8 = local_44;
      Classes_Rect_9923E97E
                (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                 *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                 *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
      AOWInterfaceImgLib_TInterfaceIL_DrawILLh_23EDC2EF
                (*(undefined4 *)(local_8 + 0x124),*(undefined4 *)(local_8 + 0x174),local_54,puVar8);
      iVar6 = *(int *)(local_8 + 0x130);
      iVar5 = (iVar6 * *(int *)(local_8 + 0x148)) / *(int *)(local_8 + 0x14c);
      iVar7 = *(int *)(local_8 + 0x134);
      if (iVar6 < iVar7 + iVar5) {
        iVar5 = iVar6 - iVar7;
      }
      if (*(char *)(local_8 + 0x161) == '\0') {
        puVar8 = local_44;
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84) + iVar5,
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),iVar7,
                   local_54,*(undefined4 *)(local_8 + 0x80));
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF
                  (*(undefined4 *)(local_8 + 0x128),*(undefined4 *)(local_8 + 0x178),local_54,puVar8
                  );
      }
      else if (*(char *)(local_8 + 0x161) == '\x01') {
        puVar8 = local_44;
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84) + iVar5,
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),iVar7,
                   local_54,*(undefined4 *)(local_8 + 0x80));
        AOWInterfaceImgLib_TInterfaceIL_DrawILLh_23EDC2EF
                  (*(undefined4 *)(local_8 + 300),*(undefined4 *)(local_8 + 0x178),local_54,puVar8);
      }
    }
    else if (cVar1 == '\x02') {
      if ((*(char *)(local_8 + 0x151) == '\x04') && (*(char *)(local_8 + 0x150) != '\0')) {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x80);
        puVar9 = local_54;
        uVar2 = *(undefined4 *)(local_8 + 0x168);
        uVar3 = *(undefined4 *)(local_8 + 0x118);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar3,uVar2);
        iVar7 = (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84)) -
                *(int *)(iVar6 + 0x20);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(local_8 + 0x118),*(undefined4 *)(local_8 + 0x168),local_8
                           ,iVar7,puVar9,uVar10,puVar8);
        Classes_Rect_9923E97E
                  (iVar7,*(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88) +
                         *(int *)(local_8 + 0x80),*(undefined4 *)(iVar6 + 0x20),puVar9,uVar10);
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF(uVar3,uVar2,local_54,puVar8);
      }
      else {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x80);
        puVar9 = local_54;
        uVar2 = *(undefined4 *)(local_8 + 0x164);
        uVar3 = *(undefined4 *)(local_8 + 0x118);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar3,uVar2);
        iVar7 = (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84)) -
                *(int *)(iVar6 + 0x20);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                          (*(undefined4 *)(local_8 + 0x118),*(undefined4 *)(local_8 + 0x164),local_8
                           ,iVar7,puVar9,uVar10,puVar8);
        Classes_Rect_9923E97E
                  (iVar7,*(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88) +
                         *(int *)(local_8 + 0x80),*(undefined4 *)(iVar6 + 0x20),puVar9,uVar10);
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF(uVar3,uVar2,local_54,puVar8);
      }
      if ((*(char *)(local_8 + 0x151) == '\x01') && (*(char *)(local_8 + 0x150) != '\0')) {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x170);
        uVar2 = *(undefined4 *)(local_8 + 0x11c);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar2,uVar10);
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(iVar6 + 0x24));
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF(uVar2,uVar10,local_54,puVar8);
      }
      else {
        puVar8 = local_44;
        uVar10 = *(undefined4 *)(local_8 + 0x16c);
        uVar2 = *(undefined4 *)(local_8 + 0x11c);
        iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(uVar2,uVar10);
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(iVar6 + 0x24));
        AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF(uVar2,uVar10,local_54,puVar8);
      }
    }
    puVar9 = puStack_74;
    puVar4 = puStack_68;
    if (*(short *)(local_8 + 0x36) != 0) {
      (**(code **)(local_8 + 0x34))(*(undefined4 *)(local_8 + 0x38),local_8,local_44);
      puVar9 = puStack_74;
      puVar4 = puStack_68;
    }
  }
  puStack_68 = puVar4;
  puStack_74 = puVar9;
  puVar9 = puStack_68;
  *in_FS_OFFSET = uStack_7c;
  *in_FS_OFFSET = uStack_70;
  puStack_68 = &LAB_5981f49d;
  puStack_6c = (undefined1 *)0x5981f495;
  System__LStrClr_40929B27(&local_58,uStack_70,puVar9);
  return;
}

