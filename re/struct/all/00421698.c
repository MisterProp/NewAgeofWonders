
void FUN_00421698(int param_1)

{
  int iVar1;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,0);
  iVar1 = *(int *)(param_1 + 0x6c);
  *(undefined1 *)(iVar1 + 0xa4) = 0;
  Sound_TSongPlayer_Play_23EDC2EF
            (*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,*(undefined4 *)(iVar1 + 0x120));
  return;
}

