
void HSEngine_TMultiHexMO_PlaceHX_23EDC2EF
               (int param_1,undefined4 param_2,undefined1 param_3,undefined1 param_4,
               undefined4 param_5)

{
                    /* 0xb18c  202  HSEngine.TMultiHexMO.PlaceHX@23EDC2EF */
  *(undefined1 *)(param_1 + 0x10) = param_3;
  *(char *)(param_1 + 0x11) = (char)param_5;
  HSEngine_TMapObject_PlaceHX_23EDC2EF(param_1,param_2,param_3,param_4,param_5);
  return;
}

