
void FUN_00427e0c(int param_1)

{
  undefined1 *puVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  undefined4 uVar6;
  int iVar7;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_00428014;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  if (*(char *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x6e) == '\0') {
    Sound_PlaySongEx_8B356FBD("Songs\\Special\\Sunset.it",2,2);
  }
  else {
    puStack_1c = &stack0xfffffffc;
    Sound_PlaySongEx_8B356FBD("Songs\\Special\\end2.s3m",2,2);
  }
  Sound_SetMusicVolume_A03DAC8A(0);
  Sound_FadeMusicVolume_2796EB69
            (*(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a158 + 0xe8) + 0x148),1);
  Engine_TByteList_Clear_23EDC2EF(*(undefined4 *)(param_1 + 0x100));
  iVar3 = (**(code **)(**(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140) + 0x54))();
  iVar3 = iVar3 + -1;
  if (0 < iVar3) {
    iVar7 = 1;
    do {
      iVar4 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),iVar7);
      if (*(char *)(iVar4 + 0xa7) != '\x04') {
        Engine_TByteList_Add_23EDC2EF(*(undefined4 *)(param_1 + 0x100),iVar7);
      }
      iVar7 = iVar7 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  iVar3 = 0;
  do {
    piVar5 = (int *)Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x104),iVar3);
    (**(code **)(*piVar5 + 0x6c))
              (piVar5,CONCAT31((int3)((uint)*(int *)(param_1 + 0x100) >> 8),
                               iVar3 < *(int *)(*(int *)(param_1 + 0x100) + 8)));
    if (iVar3 < *(int *)(*(int *)(param_1 + 0x100) + 8)) {
      uVar2 = Engine_TByteList_Get_23EDC2EF(*(int *)(param_1 + 0x100),iVar3);
      if ((uVar2 & 0xff) != (short)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5))
      goto LAB_00427f29;
      uVar6 = 1;
    }
    else {
LAB_00427f29:
      uVar6 = 0;
    }
    piVar5 = (int *)Classes_TList_Get_23EDC2EF(*(undefined4 *)(param_1 + 0x104),iVar3);
    (**(code **)(*piVar5 + 0xac))(piVar5,uVar6);
    iVar3 = iVar3 + 1;
  } while (iVar3 != 0xc);
  if (*(int *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x70) != 0) {
    iVar3 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5));
    if (*(char *)(iVar3 + 0x44) == '\x02') {
      System_LoadResString_02614E1B(AoWE_ReplayRStr_04ECFCC5_exref,&local_c);
      AoWE_TranslateRStr_435799AA(local_c,&local_8);
      AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),local_8);
      (**(code **)(**(int **)(param_1 + 0x8c) + 0x6c))(*(int **)(param_1 + 0x8c),1);
      goto LAB_00427fe7;
    }
  }
  System_LoadResString_02614E1B(AoWE_ContinueRStr_04ECFCC5_exref,&local_c);
  AoWE_TranslateRStr_435799AA(local_c,&local_8);
  AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x98),local_8);
  (**(code **)(**(int **)(param_1 + 0x8c) + 0x6c))(*(int **)(param_1 + 0x8c),0);
LAB_00427fe7:
  (**(code **)(**(int **)(DAT_0045b14c + 0x9c) + 0x6c))(*(int **)(DAT_0045b14c + 0x9c),1);
  puVar1 = puStack_20;
  *in_FS_OFFSET = 1;
  puStack_20 = &LAB_0042801b;
  uStack_24 = 0x428013;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

