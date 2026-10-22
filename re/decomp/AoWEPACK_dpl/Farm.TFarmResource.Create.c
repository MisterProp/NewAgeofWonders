
int Farm_TFarmResource_Create_23EDC2EF(int param_1,char param_2)

{
  char extraout_DL;
  char cVar1;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_18;
  
                    /* 0xb2e14  7102  Farm.TFarmResource.Create@23EDC2EF */
  cVar1 = '\0';
  if (param_2 != '\0') {
    param_1 = System__ClassCreate_51F89FF7();
    cVar1 = extraout_DL;
  }
  AoWE_TPlayerStructureResource_Create_23EDC2EF(param_1,0);
  *(undefined1 *)(param_1 + 100) = 0;
  *(undefined1 *)(param_1 + 0x58) = 1;
  *(undefined4 *)(param_1 + 0x60) = 0x32;
  *(undefined1 *)(param_1 + 0x5a) = 0x19;
  *(undefined1 *)(param_1 + 0x5b) = 2;
  if (cVar1 != '\0') {
    *in_FS_OFFSET = uStack_18;
  }
  return param_1;
}

