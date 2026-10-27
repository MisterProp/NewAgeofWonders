

void PrdPlace_TProductionPlace_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

                    /* 0xbec9c  7451  PrdPlace.TProductionPlace.ReadWrite@23EDC2EF */

  AoWE_TPlayerStructure_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0xc))(param_2,*(undefined4 *)(param_1 + 0x44),8);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x49,1,9);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x48,1,10);

  return;

}




