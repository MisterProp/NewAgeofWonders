
void AoWE_TAoWHSMap_SeatedPlayerChanged_23EDC2EF(int *param_1)

{
  undefined1 auStack_38 [48];
  
                    /* 0x76294  1458  AoWE.TAoWHSMap.SeatedPlayerChanged@23EDC2EF */
  HSEngine_THexagonSpriteSelection_UnselectAll_23EDC2EF(param_1[6]);
  Engine_InitMsg_B5A7F841(auStack_38,0x20020006,param_1);
  (**(code **)(*(int *)param_1[4] + 0x68))((int *)param_1[4],auStack_38,param_1);
  Engine_InitMsg_B5A7F841(auStack_38,0x20020009,param_1);
  (**(code **)(*(int *)param_1[4] + 0x68))((int *)param_1[4],auStack_38,param_1);
  Engine_TEventList_TriggerNotifyEvents_23EDC2EF(param_1[0x6b],AoWE_AoWHSMap_6B957A58);
  (**(code **)(*param_1 + 0x110))(param_1,0);
  return;
}

