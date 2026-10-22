
void MapCtrl_TAoWMapControl_NewTurn_23EDC2EF(int param_1)

{
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_1c;
  undefined1 *puStack_18;
  undefined1 *puStack_14;
  
                    /* 0x1062e4  9905  MapCtrl.TAoWMapControl.NewTurn@23EDC2EF */
  puStack_14 = &stack0xfffffffc;
  puStack_18 = &LAB_5580631a;
  uStack_1c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_1c;
  if (*(short *)(param_1 + 0xee) != 0) {
    (**(code **)(param_1 + 0xec))(*(undefined4 *)(param_1 + 0xf0));
  }
  *in_FS_OFFSET = uStack_1c;
  return;
}

