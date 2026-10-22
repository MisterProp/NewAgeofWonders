
void Crops_TCrop_MapFieldMsgProc_23EDC2EF(int *param_1,int *param_2)

{
                    /* 0xa5d84  6759  Crops.TCrop.MapFieldMsgProc@23EDC2EF */
  if (*param_2 != 0x10) {
    HSEngine_TMapObject_MapFieldMsgProc_23EDC2EF(param_1,param_2);
  }
  if (*param_2 == 0x1105) {
    (**(code **)(*param_1 + -4))(param_1,1);
  }
  return;
}

