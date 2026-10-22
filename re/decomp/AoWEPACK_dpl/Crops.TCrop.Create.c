
undefined4 Crops_TCrop_Create_23EDC2EF(undefined4 param_1,char param_2)

{
  char extraout_DL;
  char cVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  
                    /* 0xa5c60  6766  Crops.TCrop.Create@23EDC2EF */
  cVar1 = '\0';
  if (param_2 != '\0') {
    param_1 = System__ClassCreate_51F89FF7();
    cVar1 = extraout_DL;
  }
  ILTer_TILTerrainMO_Create_23EDC2EF(param_1,0);
  if (cVar1 != '\0') {
    *in_FS_OFFSET = uStack_18;
  }
  return param_1;
}

