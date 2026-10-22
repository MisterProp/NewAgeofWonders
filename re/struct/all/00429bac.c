
void FUN_00429bac(int param_1)

{
  FUN_00429b34(param_1);
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x34))
            (*(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc),"SONGS\\TITLE.IT");
  Sound_TSongPlayer_Play_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
  AoWBaseWin_TAOWBaseWindow_SetILTIndex_23EDC2EF(*(undefined4 *)(param_1 + 0x4c),0);
  *(undefined4 *)(param_1 + 0x54) = 0x1c2;
  return;
}

