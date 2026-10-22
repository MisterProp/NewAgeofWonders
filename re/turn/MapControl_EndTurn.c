
void MapCtrl_TAoWMapControl_EndTurn_23EDC2EF(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 extraout_ECX;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  
                    /* 0x106378  9904  MapCtrl.TAoWMapControl.EndTurn@23EDC2EF */
  puStack_14 = &stack0xfffffffc;
  puStack_18 = &LAB_558063cc;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  if (*(short *)(param_1 + 0x176) != 0) {
    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)PTR_AoWE_AoWHSMap_6B957A58_558e9494 + 0x140),param_2)
    ;
    (**(code **)(param_1 + 0x174))
              (*(undefined4 *)(param_1 + 0x178),param_2,
               CONCAT31((int3)((uint)extraout_ECX >> 8),*(undefined1 *)(iVar1 + 0x2c)) ^ 1);
  }
  *in_FS_OFFSET = uStack_1c;
  return;
}

