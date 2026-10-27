
void Gems_TGems_MapFieldMsgProc_23EDC2EF(undefined4 param_1,int *param_2)

{
                    /* 0xc52a0  7737  Gems.TGems.MapFieldMsgProc@23EDC2EF */
  HSEngine_TMapObject_MapFieldMsgProc_23EDC2EF(param_1,param_2);
  if (*param_2 == 0x1102) {
    Gems_TGems_ArmyEntered_23EDC2EF(param_1,param_2[2]);
    return;
  }
  if ((*param_2 == 0x1200) && (param_2[6] == 0)) {
    Gems_TGems_UpdateAITarget_23EDC2EF(param_1,param_2[2],param_2[3]);
    if (*(int *)param_2[3] < 0) {
      param_2[7] = 1;
    }
  }
  return;
}

