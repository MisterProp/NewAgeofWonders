
void FUN_0041de44(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uStack_18;
  undefined1 *puStack_14;
  undefined1 *puStack_10;
  undefined4 local_8;
  
  puStack_10 = &stack0xfffffffc;
  local_8 = 0;
  puStack_14 = &LAB_0041df9c;
  uStack_18 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_18;
  FUN_0041bbc4(param_1);
  uVar4 = 0;
  uVar3 = 0;
  (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xb0))();
  iVar1 = **(int **)(*(int *)PTR_DAT_0045a28c + 0x44);
  (**(code **)(iVar1 + 0x6c))(*(int **)(*(int *)PTR_DAT_0045a28c + 0x44),1,iVar1,uVar3);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x48) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a28c + 0x48),0);
  (**(code **)(**(int **)(param_1 + 0x4c) + 0x6c))(*(int **)(param_1 + 0x4c),1);
  *(undefined4 *)(param_1 + 0x148) = 0;
  *(undefined4 *)(param_1 + 0x14c) = 5;
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 100),0);
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0xf9);
  Sound_TSongPlayer_Stop_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
  SysUtils_SetCurrentDir_3C9BDBFB(*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c));
  iVar1 = FUN_0041bbb8(param_1);
  uVar3 = *(undefined4 *)(iVar1 + 0x24);
  uVar2 = FUN_0041bbb8(param_1);
  iVar1 = FUN_0040bde4(uVar2,uVar3);
  if (*(int *)(iVar1 + 0x14) == 0) {
    System__LStrLAsg_51F89FF7(&local_8,"Songs\\Title.IT");
  }
  else {
    iVar1 = FUN_0041bbb8(param_1);
    uVar3 = *(undefined4 *)(iVar1 + 0x24);
    uVar2 = FUN_0041bbb8(param_1);
    iVar1 = FUN_0040bde4(uVar2,uVar3);
    System__LStrCat3_51F89FF7(&local_8,"Songs\\",*(undefined4 *)(iVar1 + 0x14));
  }
  Sound_PlaySongEx_8B356FBD(local_8,1,0);
  iVar1 = FUN_0041bbb8(param_1);
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x5c),*(int *)(iVar1 + 0x34) + 7)
  ;
  iVar1 = FUN_0041bbb8(param_1);
  AoWImage_TAOWImage_SetCurrent_23EDC2EF(*(undefined4 *)(param_1 + 0x60),8 - *(int *)(iVar1 + 0x34))
  ;
  FUN_0041ca5c(param_1);
  *in_FS_OFFSET = 1;
  System__LStrClr_40929B27(&local_8,1,uVar4,&LAB_0041dfa3);
  return;
}

