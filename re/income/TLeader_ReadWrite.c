

void AoWE_TLeader_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

                    /* 0x8b470  496  AoWE.TLeader.ReadWrite@23EDC2EF */

  AoWE_THero_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0x44))(param_2,param_1 + 0x9c,5,0x2d);

  (**(code **)(*param_2 + 0x18))

            (param_2,0x2e,*param_2,&DAT_55702f4c,*(undefined4 *)(param_1 + 0xa0),&DAT_55702f44,

             *(undefined4 *)(param_1 + 0xa0));

  return;

}




