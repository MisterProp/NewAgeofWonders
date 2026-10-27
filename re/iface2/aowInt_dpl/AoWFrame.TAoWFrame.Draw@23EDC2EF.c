// /aowInt.dpl : AoWFrame.TAoWFrame.Draw@23EDC2EF

void AoWFrame_TAoWFrame_Draw_23EDC2EF(int param_1,undefined4 *param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  undefined1 *puVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  undefined4 *puVar11;
  undefined4 *in_FS_OFFSET;
  undefined1 *puVar12;
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
  
                    /* 0x1c4dc  475  AoWFrame.TAoWFrame.Draw@23EDC2EF */
  puStack_68 = &stack0xfffffffc;
  puStack_74 = &stack0xfffffffc;
  local_58 = 0;
  puVar11 = local_44;
  for (iVar9 = 0xf; iVar9 != 0; iVar9 = iVar9 + -1) {
    *puVar11 = *param_2;
    param_2 = param_2 + 1;
    puVar11 = puVar11 + 1;
  }
  puStack_6c = &LAB_5981c873;
  uStack_70 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_70;
  puStack_78 = &LAB_5981c82e;
  uStack_7c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_7c;
  puVar12 = &stack0xfffffffc;
  puVar4 = &stack0xfffffffc;
  local_8 = param_1;
  if ((*(char *)(param_1 + 0x2c) != '\0') &&
     (puVar12 = &stack0xfffffffc, puVar4 = &stack0xfffffffc, *(char *)(param_1 + 0x75) != '\0')) {
    puVar12 = &stack0xfffffffc;
    puVar4 = &stack0xfffffffc;
    if (*(short *)(param_1 + 0x3e) != 0) {
      (**(code **)(param_1 + 0x3c))(*(undefined4 *)(param_1 + 0x40),param_1,local_44);
      puVar12 = puStack_74;
      puVar4 = puStack_68;
    }
    puStack_68 = puVar4;
    puStack_74 = puVar12;
    cVar1 = *(char *)(local_8 + 0x114);
    if (cVar1 == '\0') {
      puVar11 = local_44;
      Classes_Rect_9923E97E
                (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                 *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                 *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
      AOWInterfaceImgLib_TInterfaceIL_DrawILF_23EDC2EF
                (*(undefined4 *)(local_8 + 0x118),*(undefined4 *)(local_8 + 0x124),local_54,puVar11)
      ;
      puVar11 = local_44;
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(local_8 + 0x118),*(int *)(local_8 + 0x124) + 1);
      iVar9 = *(int *)(local_8 + 0x80);
      iVar5 = *(int *)(iVar5 + 0x24);
      iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(local_8 + 0x118),*(int *)(local_8 + 0x124) + 6);
      iVar10 = (iVar9 - iVar5) - *(int *)(iVar6 + 0x24);
      puVar12 = local_54;
      iVar5 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(local_8 + 0x118),*(int *)(local_8 + 0x124) + 3);
      iVar9 = *(int *)(local_8 + 0x7c);
      iVar5 = *(int *)(iVar5 + 0x20);
      iVar6 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(local_8 + 0x118),*(int *)(local_8 + 0x124) + 4);
      iVar6 = *(int *)(iVar6 + 0x20);
      iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(local_8 + 0x118),*(int *)(local_8 + 0x124) + 1);
      iVar7 = *(int *)(iVar7 + 0x24);
      iVar2 = *(int *)(*(int *)(local_8 + 0x10c) + 0x88);
      iVar3 = *(int *)(local_8 + 0x88);
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                        (*(undefined4 *)(local_8 + 0x118),*(int *)(local_8 + 0x124) + 3);
      Classes_Rect_9923E97E
                (*(int *)(iVar8 + 0x20) +
                 *(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                 iVar7 + iVar2 + iVar3,(iVar9 - iVar5) - iVar6,puVar12,iVar10);
      AOWInterfaceImgLib_TInterfaceIL_DrawILT_23EDC2EF
                (*(undefined4 *)(local_8 + 0x11c),*(undefined4 *)(local_8 + 300),local_54,puVar11);
    }
    else if (cVar1 == '\x01') {
      puVar11 = local_44;
      Classes_Rect_9923E97E
                (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                 *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                 *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
      AOWInterfaceImgLib_TInterfaceIL_DrawILF_23EDC2EF
                (*(undefined4 *)(local_8 + 0x118),*(undefined4 *)(local_8 + 0x124),local_54,puVar11)
      ;
    }
    else if (cVar1 == '\x02') {
      puVar11 = local_44;
      Classes_Rect_9923E97E
                (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                 *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                 *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
      AOWInterfaceImgLib_TInterfaceIL_DrawILLh_23EDC2EF
                (*(undefined4 *)(local_8 + 0x120),*(undefined4 *)(local_8 + 0x128),local_54,puVar11)
      ;
    }
    else if (cVar1 == '\x03') {
      puVar11 = local_44;
      Classes_Rect_9923E97E
                (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                 *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                 *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
      AOWInterfaceImgLib_TInterfaceIL_DrawILLv_23EDC2EF
                (*(undefined4 *)(local_8 + 0x120),*(undefined4 *)(local_8 + 0x128),local_54,puVar11)
      ;
    }
    puVar12 = puStack_74;
    puVar4 = puStack_68;
    if (*(short *)(local_8 + 0x36) != 0) {
      (**(code **)(local_8 + 0x34))(*(undefined4 *)(local_8 + 0x38),local_8,local_44);
      puVar12 = puStack_74;
      puVar4 = puStack_68;
    }
  }
  puStack_68 = puVar4;
  puStack_74 = puVar12;
  puVar12 = puStack_68;
  *in_FS_OFFSET = uStack_7c;
  *in_FS_OFFSET = uStack_70;
  puStack_68 = &LAB_5981c87a;
  puStack_6c = (undefined1 *)0x5981c872;
  System__LStrClr_40929B27(&local_58,uStack_70,puVar12);
  return;
}

