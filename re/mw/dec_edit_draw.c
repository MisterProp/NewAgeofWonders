void AoWEdit_TAOWEdit_Draw_23EDC2EF(int param_1,undefined4 *param_2)
{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined1 *puVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  undefined4 *puVar10;
  int *piVar11;
  undefined4 *in_FS_OFFSET;
  byte bVar12;
  undefined4 uVar13;
  undefined4 uStack_74;
  undefined1 *puStack_70;
  undefined1 *puStack_6c;
  undefined4 local_5c;
  undefined4 local_58;
  int local_54 [4];
  undefined4 local_44 [4];
  int local_34 [11];
  int local_8;
                    /* 0x108ec  251  AoWEdit.TAOWEdit.Draw@23EDC2EF */
  bVar12 = 0;
  puStack_6c = &stack0xfffffffc;
  local_58 = 0;
  local_5c = 0;
  puVar10 = local_44;
  for (iVar8 = 0xf; iVar8 != 0; iVar8 = iVar8 + -1) {
    *puVar10 = *param_2;
    param_2 = param_2 + 1;
    puVar10 = puVar10 + 1;
  }
  puStack_70 = &LAB_00410ccc;
  uStack_74 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_74;
  puVar5 = &stack0xfffffffc;
  if ((*(char *)(param_1 + 0x2c) == '\x01') &&
     (puVar5 = &stack0xfffffffc, *(char *)(param_1 + 0x69) == '\x01')) {
    piVar6 = local_34;
    piVar11 = local_54;
    for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
      *piVar11 = *piVar6;
      piVar6 = piVar6 + 1;
      piVar11 = piVar11 + 1;
    }
    piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                              (*(undefined4 *)(param_1 + 0x120),0,0,local_44);
    (**(code **)(*piVar6 + 0x80))
              (piVar6,*(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80),
               *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
    uVar9 = (uint)*(byte *)(param_1 + 0x9c);
    if (-1 < (int)(uVar9 - 1)) {
      local_8 = 0;
      do {
        puVar10 = local_44;
        iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),0);
        iVar8 = *(int *)(iVar8 + 0x20);
        iVar1 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
        iVar2 = *(int *)(param_1 + 0x80);
        iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),1);
        iVar8 = iVar8 + iVar1 + iVar2 + *(int *)(iVar7 + 0x20) * local_8;
        piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF
                                  (*(undefined4 *)(param_1 + 0x120),1);
        (**(code **)(*piVar6 + 0x80))
                  (piVar6,iVar8,
                   *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),puVar10);
        local_8 = local_8 + 1;
        uVar9 = uVar9 - 1;
      } while (uVar9 != 0);
    }
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),0);
    iVar8 = *(int *)(iVar8 + 0x20);
    iVar1 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
    iVar2 = *(int *)(param_1 + 0x80);
    iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),1);
    if ((int)(iVar8 + iVar1 + iVar2 + *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c) +
             *(int *)(param_1 + 0xa0)) < local_54[2]) {
      iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),0);
      iVar8 = *(int *)(iVar8 + 0x20);
      iVar1 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
      iVar2 = *(int *)(param_1 + 0x80);
      iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),1);
      local_34[2] = iVar8 + iVar1 + iVar2 + *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c)
                    + *(int *)(param_1 + 0xa0);
    }
    puVar10 = local_44;
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),0);
    iVar8 = *(int *)(iVar8 + 0x20);
    iVar1 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
    iVar2 = *(int *)(param_1 + 0x80);
    iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),1);
    iVar8 = iVar8 + iVar1 + iVar2 + *(int *)(iVar7 + 0x20) * (uint)*(byte *)(param_1 + 0x9c);
    piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),1);
    (**(code **)(*piVar6 + 0x80))
              (piVar6,iVar8,*(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84),
               puVar10);
    piVar6 = local_54;
    piVar11 = local_34;
    for (iVar8 = 4; iVar8 != 0; iVar8 = iVar8 + -1) {
      *piVar11 = *piVar6;
      piVar6 = piVar6 + (uint)bVar12 * -2 + 1;
      piVar11 = piVar11 + (uint)bVar12 * -2 + 1;
    }
    iVar8 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),0,0,local_44)
    ;
    iVar8 = *(int *)(iVar8 + 0x20);
    iVar1 = *(int *)(*(int *)(param_1 + 0x108) + 0x80);
    iVar2 = *(int *)(param_1 + 0x80);
    iVar7 = ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),1);
    iVar7 = *(int *)(iVar7 + 0x20);
    bVar12 = *(byte *)(param_1 + 0x9c);
    iVar3 = *(int *)(param_1 + 0xa0);
    piVar6 = (int *)ImageLib_TCustomImageLibrary_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x120),2);
    (**(code **)(*piVar6 + 0x80))
              (piVar6,iVar8 + iVar1 + iVar2 + iVar7 * (uint)bVar12 + iVar3,
               *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84));
    if (*(int *)(param_1 + 0x88) != 0) {
      ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                (*(int *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x24));
      System__LStrCopy_51F89FF7
                (*(undefined4 *)(param_1 + 0x8c),*(undefined1 *)(param_1 + 0x11b),
                 *(undefined1 *)(param_1 + 0x11a),&local_58);
      ImageLib_TImageLibraryFont_SetText_23EDC2EF
                (*(undefined4 *)(param_1 + 0x88),
                 *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80) +
                 *(int *)(param_1 + 0x94),
                 *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                 *(int *)(param_1 + 0x98),local_44,local_58);
    }
    if ((*(int *)(param_1 + 0x90) != 0) && (*(int *)(param_1 + 0x114) != 0)) {
      System__LStrFromChar_001FB870(&local_58,*(undefined1 *)(param_1 + 0x111));
      puVar10 = local_44;
      uVar13 = local_58;
      System__LStrCopy_51F89FF7
                (*(undefined4 *)(param_1 + 0x8c),*(undefined1 *)(param_1 + 0x11b),
                 *(undefined4 *)(param_1 + 0x124),&local_5c);
      uVar4 = *(undefined4 *)(param_1 + 0x88);
      iVar8 = ImageLib_TImageLibraryFont_GetTextWidth_23EDC2EF(uVar4,local_5c);
      ImageLib_TImageLibraryFont_SetText_23EDC2EF
                (uVar4,iVar8 + *(int *)(*(int *)(param_1 + 0x108) + 0x80) + *(int *)(param_1 + 0x80)
                               + *(int *)(param_1 + 0x94),
                 *(int *)(*(int *)(param_1 + 0x108) + 0x84) + *(int *)(param_1 + 0x84) +
                 *(int *)(param_1 + 0x98),puVar10,uVar13);
    }
    puVar5 = puStack_6c;
    if (*(short *)(param_1 + 0x32) != 0) {
      (**(code **)(param_1 + 0x30))(*(undefined4 *)(param_1 + 0x34),param_1,local_44);
      puVar5 = puStack_6c;
    }
  }
  puStack_6c = puVar5;
  puVar5 = puStack_6c;
  *in_FS_OFFSET = uStack_74;
  puStack_6c = &LAB_00410cd3;
  puStack_70 = (undefined1 *)0x410ccb;
  System__LStrArrayClr_51F89FF7(&local_5c,2,puVar5);
  return;
}
