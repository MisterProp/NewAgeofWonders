
void AoWE_TPlayerControl_EndTurn_23EDC2EF(undefined4 param_1,char param_2)

{
  int iVar1;
  undefined1 local_4c [4];
  undefined1 local_48 [4];
  undefined1 local_44 [4];
  undefined1 local_40 [24];
  int iStack_28;
  
                    /* 0x5530c  2547  AoWE.TPlayerControl.EndTurn@23EDC2EF */
  AoWE_TAoWHSMap_LockRelationsChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58);
  if (param_2 == *(char *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5)) {
    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x50],(int)param_2);
    AoWE_TPlayer_SetEndTurn_23EDC2EF(iVar1,1);
    if (iVar1 != 0) {
      (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0xc4))
                (AoWE_AoWHSMap_6B957A58,local_4c,local_48,local_44);
      *(undefined1 *)(iVar1 + 0x2d) = local_4c[0];
      *(undefined1 *)(iVar1 + 0x2e) = local_48[0];
      *(undefined1 *)(iVar1 + 0x2f) = local_44[0];
    }
  }
  Engine_InitMsg_B5A7F841(local_40,0x20020007,param_1);
  iStack_28 = (int)param_2;
  (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0x68))(AoWE_AoWHSMap_6B957A58,local_40,param_1);
  AoWE_TAoWHSMap_UnlockRelationsChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58);
  AoWE_TAoWHSMap_TriggerRelationsChanged_23EDC2EF(AoWE_AoWHSMap_6B957A58);
  return;
}

