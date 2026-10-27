

int Crops_TPlayerCropStructure_Create_23EDC2EF(int param_1,char param_2)



{

  undefined4 uVar1;

  char extraout_DL;

  char cVar2;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_20;

  

                    /* 0xa6074  6746  Crops.TPlayerCropStructure.Create@23EDC2EF */

  cVar2 = '\0';

  if (param_2 != '\0') {

    param_1 = System__ClassCreate_51F89FF7();

    cVar2 = extraout_DL;

  }

  AoWE_TPlayerStructure_Create_23EDC2EF(param_1,0);

  *(undefined1 *)(param_1 + 0x3d) = 0;

  uVar1 = Engine_TEObject_Create_23EDC2EF(&AoWE__TPlayerStructureIncomeSource_321B7250,1,0);

  *(undefined4 *)(param_1 + 0x40) = uVar1;

  AoWE_TPlayerStructureIncomeSource_Setup_23EDC2EF(uVar1,param_1);

  if (cVar2 != '\0') {

    *in_FS_OFFSET = uStack_20;

  }

  return param_1;

}




