
void Crops_TCrop_MsgProc_23EDC2EF(undefined4 param_1,int *param_2)

{
                    /* 0xa5d60  6760  Crops.TCrop.MsgProc@23EDC2EF */
  Engine_TEObject_MsgProc_23EDC2EF(param_1,param_2);
  if (*param_2 == 0x20020007) {
    Crops_TCrop_EndTurn_23EDC2EF(param_1,(char)param_2[6]);
  }
  return;
}

