
void FUN_00454d2c(int param_1)

{
  int *piVar1;
  undefined *puVar2;
  code *pcVar3;
  undefined1 *puVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_34;
  undefined1 *puStack_30;
  undefined1 *puStack_2c;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  int local_10;
  int local_c;
  int local_8;
  
  pcVar3 = AoWE_AoWHSMap_6B957A58_exref;
  puVar2 = PTR_DAT_0045a1c0;
  puStack_2c = &stack0xfffffffc;
  local_14 = 0;
  local_18 = 0;
  local_1c = 0;
  puStack_30 = &LAB_004552e1;
  uStack_34 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_34;
  *(undefined1 *)(param_1 + 0x4c) = 0;
  local_8 = param_1;
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
  iVar5 = (**(code **)(**(int **)(*(int *)(*(int *)pcVar3 + 0x114) + 8) + 0x14))();
  if (iVar5 == 0) {
    iVar5 = (**(code **)(**(int **)(*(int *)(*(int *)pcVar3 + 0x110) + 8) + 0x14))();
    if (iVar5 == 0) {
      (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x34))
                (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),"Songs\\Heartwoo.it");
    }
    else {
      iVar5 = (**(code **)(**(int **)(*(int *)(*(int *)pcVar3 + 0x110) + 8) + 0x14))();
      if (-1 < iVar5 + -1) {
        local_c = 0;
        local_10 = iVar5;
        do {
          piVar1 = *(int **)(*(int *)(*(int *)pcVar3 + 0x110) + 0xc);
          (**(code **)(*piVar1 + 0xc))(piVar1,local_c,&local_14);
          uVar6 = local_14;
          System_LoadResString_02614E1B(AoWE_FileNotFoundRStr_8276FCC3_exref,&local_1c);
          AoWE_TranslateRStr_435799AA(local_1c,&local_18);
          iVar5 = SysUtils_CompareText_8779A639(uVar6,local_18);
          if (iVar5 != 0) {
            piVar1 = *(int **)(*(int *)(*(int *)pcVar3 + 0x110) + 8);
            (**(code **)(*piVar1 + 0xc))(piVar1,local_c,&local_14);
            (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x34))
                      (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),local_14);
          }
          local_c = local_c + 1;
          local_10 = local_10 + -1;
        } while (local_10 != 0);
        local_10 = 0;
      }
    }
  }
  else {
    (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x3c))
              (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),
               *(undefined4 *)(*(int *)(*(int *)pcVar3 + 0x114) + 8));
  }
  Sound_TSongPlayer_Play_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
  Sound_SetMusicVolume_A03DAC8A(*(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a158 + 0xe8) + 0x148));
  (**(code **)(**(int **)(*(int *)(*(int *)PTR_DAT_0045a3fc + 0x48) + 0x208) + 0x40))();
  System_LoadResString_02614E1B(PTR_PTR_0045a03c,&local_18);
  AoWE_TranslateRStr_435799AA(local_18,&local_14);
  piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a3fc + 0x48) + 0x208);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_14);
  if (*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) != 0) {
    System_LoadResString_02614E1B(PTR_PTR_0045a568,&local_18);
    AoWE_TranslateRStr_435799AA(local_18,&local_14);
    piVar1 = *(int **)(*(int *)(*(int *)PTR_DAT_0045a3fc + 0x48) + 0x208);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_14);
  }
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a434 + 0x48) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a434 + 0x48),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a28c + 0x44),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x44),1);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a5bc + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a5bc + 0x44),1);
  (**(code **)(**(int **)(*(int *)puVar2 + 0x44) + 0x6c))(*(int **)(*(int *)puVar2 + 0x44),1);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x28))
            (*(int **)(*(int *)PTR_DAT_0045a5bc + 0x48),*(undefined4 *)pcVar3);
  Scanner_TScanner_SetMap_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x5c),*(undefined4 *)pcVar3);
  uVar6 = HSEngine_TMapContainer_GetMapLevel_23EDC2EF(*(undefined4 *)(*(int *)pcVar3 + 0x10),0);
  Scanner_TScanner_SetLevel_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x5c),uVar6);
  *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a0cc + 0x5c) + 0x13c) = 0;
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x78) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x78),
             *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x1ed));
  *(undefined1 *)(*(int *)(DAT_0045b2c0 + 0x44) + 0x2d) = 0;
  iVar5 = *(int *)(*(int *)puVar2 + 0x5c);
  *(undefined4 *)(iVar5 + 0x164) = 0;
  *(undefined1 *)(iVar5 + 0x180) = 0;
  *(undefined1 *)(iVar5 + 0x2e) = 0;
  (**(code **)(**(int **)(*(int *)(*(int *)puVar2 + 0x4c) + 0x114) + 0x40))();
  System_LoadResString_02614E1B(PTR_PTR_0045a0c0,&local_18);
  AoWE_TranslateRStr_435799AA(local_18,&local_14);
  piVar1 = *(int **)(*(int *)(*(int *)puVar2 + 0x4c) + 0x114);
  (**(code **)(*piVar1 + 0x34))(piVar1,local_14);
  if (1 < *(int *)(*(int *)(*(int *)pcVar3 + 0x10) + 0x14)) {
    System_LoadResString_02614E1B(PTR_PTR_0045a288,&local_18);
    AoWE_TranslateRStr_435799AA(local_18,&local_14);
    piVar1 = *(int **)(*(int *)(*(int *)puVar2 + 0x4c) + 0x114);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_14);
  }
  if (2 < *(int *)(*(int *)(*(int *)pcVar3 + 0x10) + 0x14)) {
    System_LoadResString_02614E1B(PTR_PTR_0045a3f8,&local_18);
    AoWE_TranslateRStr_435799AA(local_18,&local_14);
    piVar1 = *(int **)(*(int *)(*(int *)puVar2 + 0x4c) + 0x114);
    (**(code **)(*piVar1 + 0x34))(piVar1,local_14);
  }
  AoWTabPanel_TAOWTabPanel_SetIndex_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x4c),0);
  *(undefined1 *)(*(int *)(*(int *)puVar2 + 0x4c) + 0x2e) = 0;
  Scanner_TScanner_SetTWidth_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x5c),0);
  Scanner_TScanner_SetTHeight_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x5c),0);
  Scanner_TScanner_SetTLeft_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x5c),0xffffffff);
  Scanner_TScanner_SetTTop_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x5c),0xffffffff);
  iVar5 = *(int *)(*(int *)puVar2 + 0x5c);
  *(undefined4 *)(iVar5 + 0x17c) = 0xffffffff;
  Scanner_TScanner_SetMap_23EDC2EF(iVar5,*(undefined4 *)pcVar3);
  uVar6 = HSEngine_TMapContainer_GetMapLevel_23EDC2EF(*(undefined4 *)(*(int *)pcVar3 + 0x10),0);
  Scanner_TScanner_SetLevel_23EDC2EF(*(undefined4 *)(*(int *)puVar2 + 0x5c),uVar6);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a5bc + 0x48) + 0x28))
            (*(int **)(*(int *)PTR_DAT_0045a5bc + 0x48),*(undefined4 *)pcVar3);
  HSEngine_THSMapViewer_SetSceneWidth_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a5bc + 0x48),
             *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x44) + 0x7c) + -0x18);
  HSEngine_THSMapViewer_SetSceneHeight_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a5bc + 0x48),
             *(int *)(*(int *)(*(int *)PTR_DAT_0045a5bc + 0x44) + 0x80) + -0x18);
  (**(code **)(**(int **)pcVar3 + 0xb0))();
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0b0 + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a0b0 + 0x44),0);
  FUN_0044ea78(*(undefined4 *)PTR_DAT_0045a130,0);
  FUN_0044ff18(*(undefined4 *)PTR_DAT_0045a130,0);
  FUN_00457508(*(undefined4 *)PTR_DAT_0045a0cc);
  FUN_00423170(*(undefined4 *)PTR_DAT_0045a4fc);
  if ((*(byte *)(*(int *)pcVar3 + 0x3c) & 4) != 0) {
    iVar5 = *(int *)(*(int *)PTR_DAT_0045a0cc + 0x5c);
    if (*(char *)(*(int *)pcVar3 + 0xa5) == -1) {
      *(undefined4 *)(iVar5 + 0x118) = 0xc;
      *(undefined4 *)(iVar5 + 0x11c) = 0xc;
      *(int *)(iVar5 + 0x140) = *(int *)(iVar5 + 0x118) * 7;
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x98) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x98),0);
    }
    else {
      iVar7 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)pcVar3 + 0x140),
                         (int)*(char *)(*(int *)pcVar3 + 0xa5));
      *(int *)(iVar5 + 0x11c) = (int)*(char *)(iVar7 + 0xa5);
      if (*(int *)(iVar5 + 0x11c) == -1) {
        *(undefined4 *)(iVar5 + 0x11c) = 0xc;
      }
      iVar7 = *(int *)(iVar5 + 0x11c);
      *(int *)(iVar5 + 0x118) = iVar7;
      *(int *)(iVar5 + 0x140) = iVar7 * 7;
      (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x98) + 0x6c))
                (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x98),
                 CONCAT31((int3)((uint)iVar7 >> 8),*(char *)(*(int *)pcVar3 + 0x239) == '\x01'));
    }
  }
  *(undefined2 *)(*(int *)PTR_DAT_0045a0cc + 0x13c) = 0;
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x78) + 0xa0))
            (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x78),0xe);
  *(undefined2 *)(*(int *)PTR_DAT_0045a0cc + 0x140) = 0;
  FUN_0040b06c(*(undefined4 *)PTR_DAT_0045a478,*(undefined4 *)(*(int *)PTR_DAT_0045a478 + 0x9c),
               *(undefined4 *)(*(int *)PTR_DAT_0045a478 + 0xa0));
  puVar4 = puStack_2c;
  *in_FS_OFFSET = uStack_34;
  puStack_2c = &LAB_004552e8;
  puStack_30 = (undefined1 *)0x4552e0;
  System__LStrArrayClr_51F89FF7(&local_1c,3,puVar4);
  return;
}

