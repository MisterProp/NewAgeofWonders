

int * AoWE_TStructureResource_Create_23EDC2EF(int *param_1,char param_2)



{

  int iVar1;

  char extraout_DL;

  char cVar2;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_18;

  

                    /* 0x6046c  2232  AoWE.TStructureResource.Create@23EDC2EF */

  cVar2 = '\0';

  if (param_2 != '\0') {

    param_1 = (int *)System__ClassCreate_51F89FF7();

    cVar2 = extraout_DL;

  }

  HSEngine_TMultiHexMOR_Create_23EDC2EF(param_1,0);

  (**(code **)(*param_1 + 100))(param_1,0x1a);

  *(undefined1 *)((int)param_1 + 0x45) = 1;

  *(undefined1 *)(param_1 + 0x11) = 1;

  iVar1 = System_TObject_Create_23EDC2EF(Classes__TStringList_D439CA30_exref,1);

  param_1[0x14] = iVar1;

  iVar1 = AoWE_TUnitIndexListCollection_Create_23EDC2EF(&AoWE__TUnitIndexListCollection_61D7FAD5,1);

  param_1[0x17] = iVar1;

  param_1[0x18] = 0x32;

  *(undefined1 *)(param_1 + 0x16) = 0;

  if (cVar2 != '\0') {

    *in_FS_OFFSET = uStack_18;

  }

  return param_1;

}




