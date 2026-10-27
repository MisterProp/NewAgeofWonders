

int AoWE_TPlayerStructureResource_Create_23EDC2EF(int param_1,char param_2)



{

  char extraout_DL;

  char cVar1;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_18;

  

                    /* 0x61974  2183  AoWE.TPlayerStructureResource.Create@23EDC2EF */

  cVar1 = '\0';

  if (param_2 != '\0') {

    param_1 = System__ClassCreate_51F89FF7();

    cVar1 = extraout_DL;

  }

  AoWE_TStructureResource_Create_23EDC2EF(param_1,0);

  *(undefined1 *)(param_1 + 100) = 1;

  *(undefined4 *)(param_1 + 0x68) = 0x14;

  *(undefined4 *)(param_1 + 0x6c) = 0xffffffdd;

  if (cVar1 != '\0') {

    *in_FS_OFFSET = uStack_18;

  }

  return param_1;

}




