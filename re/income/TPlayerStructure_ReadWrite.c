

void AoWE_TPlayerStructure_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

                    /* 0x60f44  2201  AoWE.TPlayerStructure.ReadWrite@23EDC2EF */

  AoWE_TStructure_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x30,5);

  (**(code **)(*param_2 + 0x18))

            (param_2,6,*param_2,&DAT_55702f9c,*(undefined4 *)(param_1 + 0x34),&DAT_55702f94,

             *(undefined4 *)(param_1 + 0x34));

  (**(code **)(*param_2 + 0x30))(param_2,param_1 + 0x31,7);

  return;

}




