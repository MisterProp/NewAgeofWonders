
int ILTer_TILTerrainMO_Create_23EDC2EF(int param_1,char param_2)

{
  char extraout_DL;
  char cVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  
                    /* 0x186f4  961  ILTer.TILTerrainMO.Create@23EDC2EF */
  cVar1 = '\0';
  if (param_2 != '\0') {
    param_1 = System__ClassCreate_51F89FF7();
    cVar1 = extraout_DL;
  }
  Engine_TEObject_Create_23EDC2EF(param_1,0);
  *(undefined1 *)(param_1 + 0x18) = 0xff;
  if (cVar1 != '\0') {
    *in_FS_OFFSET = uStack_18;
  }
  return param_1;
}

