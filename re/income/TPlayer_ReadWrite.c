

void AoWE_TPlayer_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

  int iVar1;

  uint uVar2;

  undefined1 uVar3;

  undefined4 uStack_20;

  undefined4 uStack_1c;

  undefined1 auStack_18 [12];

  

                    /* 0x5314c  2597  AoWE.TPlayer.ReadWrite@23EDC2EF */

  uStack_1c = 0x5575315f;

  Engine_TEObject_ReadWrite_23EDC2EF(param_1,param_2);

  uStack_1c = 0x55753171;

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0xa5,2);

  uStack_1c = 3;

  uStack_20 = 0x55753185;

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0xa7,1);

  uStack_20 = 0x55753197;

  (**(code **)(*param_2 + 0x38))(param_2,param_1 + 0xa9,4);

  uStack_20 = 0x557531a9;

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0xac,5);

  uStack_20 = 0x557531bb;

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0xb0,6);

  uStack_20 = 10;

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x45,1);

  uVar2 = (**(code **)*param_2)();

  if ((uVar2 & 4) == 0) {

    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0xbc,0xb);

  }

  (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0xc0,0xc);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0xc4,0x10);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0xa6,0x12);

  if ((*(byte *)(AoWE_AoWEngine_7E705837 + 0x34) & 6) == 0) {

    uVar2 = (**(code **)*param_2)();

    if ((uVar2 & 4) == 0) goto LAB_5575328f;

  }

  uVar2 = (**(code **)*param_2)();

  if ((uVar2 & 2) == 0) {

    (**(code **)(*param_2 + 8))(param_2,param_1 + 0xd4,0,0x13);

    if (*(int *)(param_1 + 0xd4) != 0) {

      uVar2 = (**(code **)*param_2)();

      if ((uVar2 & 8) == 0) {

        iVar1 = *(int *)(param_1 + 0xd4);

        *(undefined1 *)(iVar1 + 0x24) = *(undefined1 *)(param_1 + 0xa6);

        AoWE_THeroControl_RegisterHero_23EDC2EF(AoWE_AoWHSMap_6B957A58[0x3e],iVar1);

      }

    }

  }

LAB_5575328f:

  uVar2 = (**(code **)*param_2)();

  if ((uVar2 & 2) == 0) {

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2d,0x14);

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2e,0x15);

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2f,0x16);

  }

  else if ((*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0xa5) == *(char *)(param_1 + 0xa6)) &&

          (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x13a) != '\x01')) {

    uVar3 = SUB41(auStack_18,0);

    (**(code **)(*AoWE_AoWHSMap_6B957A58 + 0xc4))(AoWE_AoWHSMap_6B957A58,&uStack_20,&uStack_1c);

    *(undefined1 *)(param_1 + 0x2d) = uVar3;

    *(undefined1 *)(param_1 + 0x2e) = (undefined1)uStack_20;

    *(undefined1 *)(param_1 + 0x2f) = (undefined1)uStack_1c;

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2d,0x14);

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2e,0x15);

    (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x2f,0x16);

  }

  uVar2 = (**(code **)*param_2)();

  if ((uVar2 & 8) == 0) {

    (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0xd8),0x17);

  }

  (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x30,0x18);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x44,1,0x19);

  uVar2 = (**(code **)*param_2)();

  if ((uVar2 & 1) != 0) {

    (**(code **)(*param_2 + 0x18))

              (param_2,0x1a,*param_2,&DAT_55702f4c,*(undefined4 *)(param_1 + 0x3c),&DAT_55702f44,

               *(undefined4 *)(param_1 + 0x3c));

  }

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x50,0x1b);

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x88),0x1c);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0xa8,1,0x1d);

  if (*(char *)(param_1 + 0xa6) != '\0') {

    uVar2 = (**(code **)*param_2)();

    if ((((uVar2 & 2) == 0) || ((*(byte *)(AoWE_AoWHSMap_6B957A58 + 0xf) & 2) == 0)) ||

       (*(int *)(param_1 + 0xd4) == 0)) {

      (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x58),0x1e);

    }

    else {

      (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0xd4),0x1e);

    }

  }

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x54),0x1f);

  (**(code **)(*param_2 + 0x18))

            (param_2,0x20,*param_2,Engine_TIntegerList_SaveToStream_23EDC2EF,

             *(undefined4 *)(param_1 + 0x98),Engine_TIntegerList_LoadFromStream_23EDC2EF,

             *(undefined4 *)(param_1 + 0x98));

  (**(code **)(*param_2 + 0x18))

            (param_2,0x21,*param_2,Engine_TIntegerList_SaveToStream_23EDC2EF,

             *(undefined4 *)(param_1 + 0x9c),Engine_TIntegerList_LoadFromStream_23EDC2EF,

             *(undefined4 *)(param_1 + 0x9c));

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x5c,0x22);

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x60),0x23);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x6c,0x24);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x70,0x25);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x78,0x26);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x7c,0x27);

  uVar2 = (**(code **)*param_2)();

  if ((uVar2 & 1) != 0) {

    AoWE_TPlayer_SetRace_23EDC2EF(param_1,*(undefined1 *)(param_1 + 0xa5));

    *(undefined1 *)(*(int *)(param_1 + 0xd8) + 0xc) = *(undefined1 *)(param_1 + 0xa6);

    *(undefined1 *)(*(int *)(param_1 + 0x3c) + 0x10) = 3;

  }

  (**(code **)(*param_2 + 0x24))(param_2,*(undefined4 *)(param_1 + 0x34),0x28);

  uVar2 = (**(code **)*param_2)();

  if ((uVar2 & 1) != 0) {

    (**(code **)(*param_2 + 0x18))

              (param_2,0x29,*param_2,&DAT_55702f4c,*(undefined4 *)(param_1 + 0x38),&DAT_55702f44,

               *(undefined4 *)(param_1 + 0x38));

  }

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x74,0x32);

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x8c),0x33);

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x90),0x34);

  (**(code **)(*param_2 + 0x18))

            (param_2,0x35,*param_2,&DAT_55702f9c,*(undefined4 *)(param_1 + 0x94),&DAT_55702f94,

             *(undefined4 *)(param_1 + 0x94));

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0xa0),0x36);

  (**(code **)(*param_2 + 0x38))(param_2,param_1 + 0xa4,0x38);

  uVar2 = (**(code **)*param_2)();

  if (((uVar2 & 1) != 0) || (*(char *)((int)AoWE_AoWHSMap_6B957A58 + 0x13a) != '\x01')) {

    (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0xb4,0x39);

  }

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x40),0x3a);

  if (*(char *)(param_1 + 0xa6) == '\0') {

    *(undefined1 *)(param_1 + 0xa7) = 4;

  }

  return;

}




