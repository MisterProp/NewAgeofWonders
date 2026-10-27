

int AoWE_TPlayerStructure_Create_23EDC2EF(int param_1,char param_2)



{

  undefined4 uVar1;

  char extraout_DL;

  char cVar2;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_18;

  

                    /* 0x60734  2224  AoWE.TPlayerStructure.Create@23EDC2EF */

  cVar2 = '\0';

  if (param_2 != '\0') {

    param_1 = System__ClassCreate_51F89FF7();

    cVar2 = extraout_DL;

  }

  AoWE_TStructure_Create_23EDC2EF(param_1,0);

  uVar1 = System_TObject_Create_23EDC2EF(Engine__TBitList_7F8938B7_exref,1);

  *(undefined4 *)(param_1 + 0x34) = uVar1;

  *(undefined1 *)(param_1 + 0x38) = 0;

  if (cVar2 != '\0') {

    *in_FS_OFFSET = uStack_18;

  }

  return param_1;

}




