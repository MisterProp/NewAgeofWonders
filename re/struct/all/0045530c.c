
void FUN_0045530c(void)

{
  (**(code **)(**(int **)AoWE_AoWEngine_7E705837_exref + 0xb0))();
  AoWE_TAoWEngine_CloseCampaign_23EDC2EF(*(undefined4 *)AoWE_AoWEngine_7E705837_exref);
  Scanner_TScanner_SetMap_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a1c0 + 0x5c),0);
  Scanner_TScanner_SetMap_23EDC2EF(*(undefined4 *)(*(int *)PTR_DAT_0045a098 + 0x4c),0);
  Sound_TSongPlayer_Stop_23EDC2EF(*(undefined4 *)Sound_SongPlayer_F3DCC77F_exref,0);
  (**(code **)(**(int **)(*(int *)Sound_SongPlayer_F3DCC77F_exref + 0xc) + 0x40))();
  SysUtils_SetCurrentDir_3C9BDBFB(*(undefined4 *)(*(int *)AoWE_AoWEngine_7E705837_exref + 0x2c));
  FUN_00455a34(DAT_0045b2c0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a0cc + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a0cc + 0x44),0);
  (**(code **)(**(int **)(*(int *)PTR_DAT_0045a28c + 0x44) + 0x6c))
            (*(int **)(*(int *)PTR_DAT_0045a28c + 0x44),0);
  FUN_0041a128(*(undefined4 *)PTR_DAT_0045a434);
  FUN_0041a150(*(undefined4 *)PTR_DAT_0045a434,1);
  return;
}

