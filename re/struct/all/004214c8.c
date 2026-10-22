
void FUN_004214c8(int param_1)

{
  int *piVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  int local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_c = 0;
  puStack_20 = &LAB_00421689;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  (**(code **)(**(int **)(*(int *)(param_1 + 0x6c) + 0x114) + 0x40))();
  iVar3 = (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x10) + 0x14))();
  if (-1 < iVar3 + -1) {
    local_8 = 0;
    do {
      (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x10) + 0xc))
                (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x10),local_8,&local_c);
      piVar1 = *(int **)(*(int *)(param_1 + 0x6c) + 0x114);
      (**(code **)(*piVar1 + 0x34))(piVar1,local_c);
      local_8 = local_8 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  AoWListBox_TAOWListBox_SetIndex_23EDC2EF
            (*(undefined4 *)(param_1 + 0x6c),
             *(undefined4 *)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x14));
  iVar3 = *(int *)(param_1 + 0x6c);
  *(undefined1 *)(iVar3 + 0x2e) = 0;
  if (*(int *)(iVar3 + 0x120) == -1) {
    (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),0);
    (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),0);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x80),0);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x74) + 0x6c))(*(int **)(param_1 + 0x74),1);
    (**(code **)(**(int **)(param_1 + 0x78) + 0x6c))(*(int **)(param_1 + 0x78),1);
    (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x10) + 0xc))
              (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x10),
               *(undefined4 *)(*(int *)(param_1 + 0x6c) + 0x120),&local_c);
    AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x80),local_c);
  }
  *(undefined1 *)(*(int *)(param_1 + 0x90) + 0xa4) = 0;
  *(undefined1 *)(*(int *)(param_1 + 100) + 0xa4) = 0;
  (**(code **)(**(int **)(param_1 + 200) + 0xac))
            (*(int **)(param_1 + 200),*(undefined1 *)(param_1 + 0x11c));
  (**(code **)(**(int **)(param_1 + 0xd0) + 0xac))
            (*(int **)(param_1 + 0xd0),*(undefined1 *)(param_1 + 0x124));
  (**(code **)(**(int **)(param_1 + 0xfc) + 0xac))
            (*(int **)(param_1 + 0xfc),*(undefined1 *)(param_1 + 0x11d));
  (**(code **)(**(int **)(param_1 + 0xa4) + 0xac))
            (*(int **)(param_1 + 0xa4),*(int *)(param_1 + 0x110) == 0);
  (**(code **)(**(int **)(param_1 + 0xac) + 0xac))
            (*(int **)(param_1 + 0xac),*(int *)(param_1 + 0x110) == 1);
  (**(code **)(**(int **)(param_1 + 0xb4) + 0xac))
            (*(int **)(param_1 + 0xb4),*(int *)(param_1 + 0x110) == 2);
  (**(code **)(**(int **)(param_1 + 0xbc) + 0xac))
            (*(int **)(param_1 + 0xbc),*(int *)(param_1 + 0x110) == 3);
  puVar2 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_00421690;
  puStack_20 = (undefined1 *)0x421688;
  System__LStrClr_40929B27(&local_c,uStack_24,puVar2);
  return;
}

