

int * HSEngine_TMapObjectResource_Create_23EDC2EF(int *param_1,char param_2)



{

  int iVar1;

  char extraout_DL;

  char cVar2;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_18;

  

                    /* 0x9e64  263  HSEngine.TMapObjectResource.Create@23EDC2EF */

  cVar2 = '\0';

  if (param_2 != '\0') {

    param_1 = (int *)System__ClassCreate_51F89FF7();

    cVar2 = extraout_DL;

  }

  Engine_TEObject_Create_23EDC2EF(param_1,0);

  iVar1 = System_TObject_Create_23EDC2EF(&HSEngine__TTerrainList_7EE8CC88,1);

  param_1[10] = iVar1;

  iVar1 = ImageLib_TCustomImageLibrary_Create_23EDC2EF(ImageLib__TImageLibrary_FA2681AB_exref,1,0);

  param_1[8] = iVar1;

  (**(code **)(*param_1 + 100))(param_1,0x1a);

  if (cVar2 != '\0') {

    *in_FS_OFFSET = uStack_18;

  }

  return param_1;

}




