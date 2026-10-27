

bool HSEngine_TMapObject_LinkToResource_23EDC2EF(int *param_1,undefined4 param_2)



{

  undefined4 uVar1;

  int iVar2;

  

                    /* 0x96b8  292  HSEngine.TMapObject.LinkToResource@23EDC2EF */

  if ((*(byte *)(param_1 + 3) & 2) == 0) {

    uVar1 = (**(code **)(*param_1 + 0x108))(param_1,param_2);

    iVar2 = Engine_TEResourceList_FindResource_23EDC2EF(uVar1,param_1[2]);

    if (iVar2 != 0) {

      param_1[2] = iVar2;

      *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) | 2;

    }

  }

  return (*(byte *)(param_1 + 3) & 2) != 0;

}




