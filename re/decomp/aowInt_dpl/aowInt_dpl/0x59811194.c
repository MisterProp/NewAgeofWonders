// /aowInt.dpl : 0x59811194

void AoWButton_TAOWBaseButton_Draw_23EDC2EF(int param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar8;
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
  
                    /* 0x11194  263  AoWButton.TAOWBaseButton.Draw@23EDC2EF */
  puStack_68 = &stack0xfffffffc;
  puStack_74 = &stack0xfffffffc;
  local_58 = 0;
  puVar7 = local_44;
  for (iVar5 = 0xf; iVar5 != 0; iVar5 = iVar5 + -1) {
    *puVar7 = *param_2;
    param_2 = param_2 + 1;
    puVar7 = puVar7 + 1;
  }
  puStack_6c = &LAB_59811629;
  uStack_70 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_70;
  puStack_78 = &LAB_598115e4;
  uStack_7c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_7c;
  puVar2 = &stack0xfffffffc;
  puVar3 = &stack0xfffffffc;
  local_8 = param_1;
  if ((*(char *)(param_1 + 0x75) != '\0') &&
     (puVar2 = &stack0xfffffffc, puVar3 = &stack0xfffffffc, *(char *)(param_1 + 0x2c) != '\0')) {
    puVar2 = &stack0xfffffffc;
    puVar3 = &stack0xfffffffc;
    if (*(short *)(param_1 + 0x3e) != 0) {
      (**(code **)(param_1 + 0x3c))(*(undefined4 *)(param_1 + 0x40),param_1,local_44);
      puVar2 = puStack_74;
      puVar3 = puStack_68;
    }
    puStack_68 = puVar3;
    puStack_74 = puVar2;
    if (*(char *)(local_8 + 0xb0) == '\0') {
      if (*(char *)(local_8 + 0x118) == '\0') {
        if (*(char *)(local_8 + 0x134) == '\0') {
          puVar7 = local_44;
          Classes_Rect_9923E97E
                    (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                     *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                     *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
          AOWInterfaceImgLib_TInterfaceIL_DrawILLh_23EDC2EF
                    (*(undefined4 *)(local_8 + 0x11c),*(undefined4 *)(local_8 + 0x124),local_54,
                     puVar7);
        }
        else {
          puVar7 = local_44;
          Classes_Rect_9923E97E
                    (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                     *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                     *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
          AOWInterfaceImgLib_TInterfaceIL_DrawILLh_23EDC2EF
                    (*(undefined4 *)(local_8 + 0x11c),*(undefined4 *)(local_8 + 300),local_54,puVar7
                    );
        }
      }
      else if (*(char *)(local_8 + 0x134) == '\0') {
        puVar7 = local_44;
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
        AOWInterfaceImgLib_TInterfaceIL_DrawILI_23EDC2EF
                  (*(undefined4 *)(local_8 + 0x120),*(undefined4 *)(local_8 + 0x124),local_54,puVar7
                  );
      }
      else {
        puVar7 = local_44;
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
        AOWInterfaceImgLib_TInterfaceIL_DrawILI_23EDC2EF
                  (*(undefined4 *)(local_8 + 0x120),*(undefined4 *)(local_8 + 300),local_54,puVar7);
      }
      if (*(int *)(local_8 + 0x8c) != 0) {
        ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                  (*(int *)(local_8 + 0x8c),*(undefined4 *)(local_8 + 0x24));
        uVar8 = *(undefined4 *)(local_8 + 0x90);
        puVar7 = local_44;
        uVar1 = *(undefined4 *)(local_8 + 0x8c);
        uVar4 = ImageLib_TImageLibraryFont_GetTextWidth_23EDC2EF
                          (uVar1,*(undefined4 *)(local_8 + 0x90));
        iVar5 = (int)uVar4 >> 1;
        if (iVar5 < 0) {
          iVar5 = iVar5 + (uint)((uVar4 & 1) != 0);
        }
        iVar6 = (int)*(uint *)(local_8 + 0x7c) >> 1;
        if (iVar6 < 0) {
          iVar6 = iVar6 + (uint)((*(uint *)(local_8 + 0x7c) & 1) != 0);
        }
        ImageLib_TImageLibraryFont_SetText_23EDC2EF
                  (uVar1,(*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84) +
                         iVar6) - iVar5,
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88) +
                   *(int *)(local_8 + 0x9c),puVar7,uVar8);
      }
    }
    else {
      if (*(char *)(local_8 + 0x118) == '\0') {
        puVar7 = local_44;
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
        AOWInterfaceImgLib_TInterfaceIL_DrawILLh_23EDC2EF
                  (*(undefined4 *)(local_8 + 0x11c),*(undefined4 *)(local_8 + 0x128),local_54,puVar7
                  );
      }
      else {
        puVar7 = local_44;
        Classes_Rect_9923E97E
                  (*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84),
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88),
                   *(undefined4 *)(local_8 + 0x7c),local_54,*(undefined4 *)(local_8 + 0x80));
        AOWInterfaceImgLib_TInterfaceIL_DrawILI_23EDC2EF
                  (*(undefined4 *)(local_8 + 0x120),*(undefined4 *)(local_8 + 0x128),local_54,puVar7
                  );
      }
      if (*(int *)(local_8 + 0x8c) != 0) {
        ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                  (*(int *)(local_8 + 0x8c),*(undefined4 *)(local_8 + 0x28));
        uVar8 = *(undefined4 *)(local_8 + 0x90);
        puVar7 = local_44;
        uVar1 = *(undefined4 *)(local_8 + 0x8c);
        uVar4 = ImageLib_TImageLibraryFont_GetTextWidth_23EDC2EF
                          (uVar1,*(undefined4 *)(local_8 + 0x90));
        iVar5 = (int)uVar4 >> 1;
        if (iVar5 < 0) {
          iVar5 = iVar5 + (uint)((uVar4 & 1) != 0);
        }
        iVar6 = (int)*(uint *)(local_8 + 0x7c) >> 1;
        if (iVar6 < 0) {
          iVar6 = iVar6 + (uint)((*(uint *)(local_8 + 0x7c) & 1) != 0);
        }
        ImageLib_TImageLibraryFont_SetText_23EDC2EF
                  (uVar1,(*(int *)(*(int *)(local_8 + 0x10c) + 0x84) + *(int *)(local_8 + 0x84) +
                         iVar6) - iVar5,
                   *(int *)(*(int *)(local_8 + 0x10c) + 0x88) + *(int *)(local_8 + 0x88) +
                   *(int *)(local_8 + 0x114),puVar7,uVar8);
      }
    }
    puVar2 = puStack_74;
    puVar3 = puStack_68;
    if (*(short *)(local_8 + 0x36) != 0) {
      (**(code **)(local_8 + 0x34))(*(undefined4 *)(local_8 + 0x38),local_8,local_44);
      puVar2 = puStack_74;
      puVar3 = puStack_68;
    }
  }
  puStack_68 = puVar3;
  puStack_74 = puVar2;
  puVar2 = puStack_68;
  *in_FS_OFFSET = uStack_7c;
  *in_FS_OFFSET = uStack_70;
  puStack_68 = &LAB_59811630;
  puStack_6c = (undefined1 *)0x59811628;
  System__LStrClr_40929B27(&local_58,uStack_70,puVar2);
  return;
}

