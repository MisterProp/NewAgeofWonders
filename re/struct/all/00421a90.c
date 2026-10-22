
void FUN_00421a90(int param_1)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00421afa;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  (**(code **)(*DAT_0045b120 + 0xc))
            (DAT_0045b120,*(undefined4 *)(*(int *)(param_1 + 0x58) + 0x120),&local_8);
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x34))
            (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),local_8);
  FUN_004214c8(param_1);
  puVar1 = puStack_14;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_00421b01;
  puStack_18 = (undefined1 *)0x421af9;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

