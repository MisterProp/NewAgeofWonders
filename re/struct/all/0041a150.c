
void FUN_0041a150(int param_1,char param_2)

{
  int iVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined1 uVar4;
  undefined4 uStack_20;
  undefined1 *puStack_1c;
  undefined1 *puStack_18;
  undefined4 local_c;
  char local_5;
  
  puStack_18 = &stack0xfffffffc;
  local_c = 0;
  puStack_1c = &LAB_0041a2dc;
  uStack_20 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_20;
  puVar2 = &stack0xfffffffc;
  local_5 = param_2;
  if (param_2 != '\0') {
    iVar1 = *(int *)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x14);
    puVar2 = &stack0xfffffffc;
    if (iVar1 != -1) {
      iVar3 = (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x14))();
      uVar4 = iVar1 == iVar3;
      puVar2 = puStack_18;
      if (iVar1 < iVar3) {
        (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0xc))
                  (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),
                   *(undefined4 *)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x14),&local_c);
        System__LStrCmp_51F89FF7(local_c,"SONGS\\TITLE.IT");
        puVar2 = puStack_18;
        if ((bool)uVar4) goto LAB_0041a1fd;
      }
    }
    puStack_18 = puVar2;
    (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
    (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x34))
              (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),"SONGS\\TITLE.IT");
    Sound_TSongPlayer_Play_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
    puVar2 = puStack_18;
  }
LAB_0041a1fd:
  puStack_18 = puVar2;
  AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
            (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0);
  ImageLib_TCustomImageLibrary_LoadDiscardableImageData_23EDC2EF(*(undefined4 *)(param_1 + 0x44));
  FUN_0040b06c(*(undefined4 *)PTR_DAT_0045a478,0x280,0x1e0);
  FUN_0040b034(*(undefined4 *)PTR_DAT_0045a478,2);
  FUN_0040b390(*(undefined4 *)PTR_DAT_0045a478);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a168 + 0x1dc) + 0x84))
            (*(int **)(*(int *)PTR_DAT_0045a168 + 0x1dc),0x1e);
  *(undefined1 *)(*(int *)(*(int *)PTR_DAT_0045a168 + 0x1e4) + 0x59) = 1;
  *(undefined1 *)(param_1 + 0x94) = 1;
  *(undefined4 *)(param_1 + 0x7c) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x80) = 0xffffffff;
  *(undefined4 *)(param_1 + 0xac) = *(undefined4 *)(*(int *)PTR_DAT_0045a210 + 0x5c);
  *(undefined4 *)(param_1 + 0xb0) = *(undefined4 *)(*(int *)PTR_DAT_0045a210 + 0x5c);
  AOWWinManager_TAOWWinManager_SetILBIndex_23EDC2EF
            (*(undefined4 *)(*(int *)(param_1 + 0x48) + 0x1d8),4);
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),1);
  puVar2 = puStack_18;
  *in_FS_OFFSET = uStack_20;
  puStack_18 = &LAB_0041a2e3;
  puStack_1c = (undefined1 *)0x41a2db;
  System__LStrClr_40929B27(&local_c,uStack_20,puVar2);
  return;
}

