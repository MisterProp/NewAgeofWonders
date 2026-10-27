

void AoWE_TPlayerMagicControl_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

                    /* 0x7c748  1209  AoWE.TPlayerMagicControl.ReadWrite@23EDC2EF */

  Engine_TEObject_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x24,0x14);

  (**(code **)(*param_2 + 0x40))(param_2,param_1 + 0x2c,0x32,0x15);

  (**(code **)(*param_2 + 0x18))

            (param_2,0x16,*param_2,Engine_TIntegerList_SaveToStream_23EDC2EF,

             *(undefined4 *)(param_1 + 0x30),Engine_TIntegerList_LoadFromStream_23EDC2EF,

             *(undefined4 *)(param_1 + 0x30));

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x34,0x17);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x38,0x18);

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x3c),0x19);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x44,1,0x1a);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x18,0x1b);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 0x1c,0x1c);

  (**(code **)(*param_2 + 0x38))(param_2,param_1 + 0xc,0x1d);

  (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 8,0x1e);

  return;

}




