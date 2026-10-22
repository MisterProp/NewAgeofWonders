
void FUN_004216c8(int param_1)

{
  undefined1 *puVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  undefined4 local_8;
  
  puStack_14 = &stack0xfffffffc;
  local_8 = 0;
  puStack_18 = &LAB_00421736;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x10) + 0xc))
            (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x10),
             *(undefined4 *)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0x14),&local_8);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x80),local_8);
  puVar1 = puStack_14;
  *(undefined1 *)(*(int *)(param_1 + 0x90) + 0xa4) = 0;
  *(undefined1 *)(*(int *)(param_1 + 0x6c) + 0xa4) = 0;
  *in_FS_OFFSET = uStack_1c;
  puStack_14 = &LAB_0042173d;
  puStack_18 = (undefined1 *)0x421735;
  System__LStrClr_40929B27(&local_8,uStack_1c,puVar1);
  return;
}

