
void FUN_0042abf0(int param_1,undefined4 param_2)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined1 uVar5;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  undefined4 local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_c = 0;
  puStack_20 = &LAB_0042add0;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  iVar3 = *(int *)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x14);
  puVar1 = &stack0xfffffffc;
  local_8 = param_2;
  if (iVar3 != -1) {
    iVar2 = (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x14))();
    uVar5 = iVar3 == iVar2;
    puVar1 = puStack_1c;
    if (iVar3 < iVar2) {
      (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0xc))
                (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),
                 *(undefined4 *)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x14),&local_c);
      System__LStrCmp_51F89FF7(local_c,"SONGS\\TITLE.IT");
      puVar1 = puStack_1c;
      if ((bool)uVar5) goto LAB_0042ac94;
    }
  }
  puStack_1c = puVar1;
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x34))
            (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),"SONGS\\TITLE.IT");
  Sound_TSongPlayer_Play_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
LAB_0042ac94:
  FUN_0040b06c(*(undefined4 *)PTR_DAT_0045a478,0x280,0x1e0);
  System__LStrAsg_51F89FF7(param_1 + 0xec,local_8);
  if (*(char *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x1ed) != '\0') {
    aowDPlayComp_TGDPlay_DisConnect_23EDC2EF
              (*(int *)(*(int *)PTR_DAT_0045a454 + 0x44),PTR_DAT_0045a454);
  }
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a28c + 0x44),1);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x48) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a28c + 0x48),0);
  (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),1);
  (**(code **)(**(int **)(param_1 + 0x5c) + 0x6c))(*(int **)(param_1 + 0x5c),1);
  (**(code **)(**(int **)(param_1 + 0x70) + 0x6c))(*(int **)(param_1 + 0x70),0);
  (**(code **)(**(int **)(param_1 + 0x98) + 0x6c))(*(int **)(param_1 + 0x98),0);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),0);
  (**(code **)(**(int **)(param_1 + 0x60) + 0x6c))(*(int **)(param_1 + 0x60),0);
  (**(code **)(**(int **)(param_1 + 100) + 0x6c))(*(int **)(param_1 + 100),0);
  (**(code **)(**(int **)(param_1 + 0xcc) + 0x6c))(*(int **)(param_1 + 0xcc),0);
  iVar3 = aowDPlayComp_TGDPlay_GetConnections_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_DAT_0045a454 + 0x44));
  if ((iVar3 == 0) &&
     (iVar3 = *(int *)(*(int *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x1bc) + 8),
     -1 < iVar3 + -1)) {
    iVar2 = 0;
    do {
      iVar4 = Classes_TList_Get_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)PTR_DAT_0045a454 + 0x44) + 0x1bc),iVar2);
      if (*(char *)(iVar4 + 0x1c) == '\x02') {
        (**(code **)(**(int **)(param_1 + 0x60) + 0x6c))(*(int **)(param_1 + 0x60),1);
      }
      else if (*(char *)(iVar4 + 0x1c) == '\x03') {
        (**(code **)(**(int **)(param_1 + 100) + 0x6c))(*(int **)(param_1 + 100),1);
      }
      iVar2 = iVar2 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  puVar1 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_0042add7;
  puStack_20 = (undefined1 *)0x42adcf;
  System__LStrClr_40929B27(&local_c,uStack_24,puVar1);
  return;
}

