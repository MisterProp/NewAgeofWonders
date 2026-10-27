

void HSEngine_TMapObjectResource_ReadWrite_23EDC2EF(int param_1,int *param_2,undefined4 param_3)



{

  uint uVar1;

  undefined4 uVar2;

  undefined4 uStack_30;

  int *piStack_2c;

  undefined4 uStack_28;

  int *piStack_24;

  undefined4 uStack_20;

  int *piStack_1c;

  undefined4 uStack_18;

  int *piStack_14;

  undefined4 uStack_10;

  

                    /* 0xa020  254  HSEngine.TMapObjectResource.ReadWrite@23EDC2EF */

  piStack_14 = (int *)0x5560a031;

  uStack_10 = param_3;

  Engine_TEResource_ReadWrite_23EDC2EF(param_1,param_2);

  piStack_14 = (int *)0x5560a040;

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x1c,4);

  piStack_14 = (int *)0x5560a04f;

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2c,5);

  piStack_14 = (int *)0x5560a05e;

  (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x30,6);

  piStack_1c = *(int **)(param_1 + 0x28);

  uStack_18 = *(undefined4 *)(*piStack_1c + 4);

  uStack_20 = *(undefined4 *)(*piStack_1c + 8);

  piStack_24 = (int *)0x5560a07b;

  piStack_14 = piStack_1c;

  (**(code **)(*param_2 + 0x18))(param_2,7);

  piStack_24 = (int *)0x5560a083;

  ImageLib_TCustomImageLibrary_ReduceCount_23EDC2EF(*(undefined4 *)(param_1 + 0x20));

  piStack_2c = *(int **)(param_1 + 0x20);

  uStack_28 = *(undefined4 *)(*piStack_2c + 0x34);

  uStack_30 = *(undefined4 *)(*piStack_2c + 0x38);

  piStack_24 = piStack_2c;

  (**(code **)(*param_2 + 0x18))(param_2,8);

  uVar1 = (**(code **)*param_2)();

  if ((uVar1 & 4) == 0) {

    uVar1 = (**(code **)*param_2)();

    if ((uVar1 & 1) == 0) {

      uStack_30 = CONCAT31(uStack_30._1_3_,*(int *)(param_1 + 0x24) != 0);

      (**(code **)(*param_2 + 0x38))(param_2,&uStack_30,9);

    }

    else {

      (**(code **)(*param_2 + 0x38))(param_2,&uStack_30,9);

      if ((char)uStack_30 == '\0') {

        System_TObject_Free_23EDC2EF(*(undefined4 *)(param_1 + 0x24));

        *(undefined4 *)(param_1 + 0x24) = 0;

      }

      else if (*(int *)(param_1 + 0x24) == 0) {

        uVar2 = ImageLib_TImageSequenceList_Create_23EDC2EF

                          (ImageLib__TImageSequenceList_C32858A4_exref,1,0,

                           *(undefined4 *)(param_1 + 0x20));

        *(undefined4 *)(param_1 + 0x24) = uVar2;

      }

    }

    if ((char)uStack_30 != '\0') {

      (**(code **)(*param_2 + 0x18))

                (param_2,10,*param_2,&DAT_5560197c,*(undefined4 *)(param_1 + 0x24),&DAT_55601974,

                 *(undefined4 *)(param_1 + 0x24));

    }

  }

  return;

}




