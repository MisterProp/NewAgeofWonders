
undefined4 Crops_TCrop_Destroy_23EDC2EF(undefined4 param_1,uint param_2)

{
                    /* 0xa5c90  6765  Crops.TCrop.Destroy@23EDC2EF */
  HSEngine_THexagonSprite_Destroy_23EDC2EF(param_1,param_2 & 0xfffffffe);
  if ((char)param_2 != '\0') {
    System__ClassDestroy_51F89FF7(param_1);
  }
  return param_1;
}

