

void PowerNode_TPowerNode_ReadWrite_23EDC2EF(int param_1,int *param_2)



{

                    /* 0xd0440  8162  PowerNode.TPowerNode.ReadWrite@23EDC2EF */

  AoWE_TPlayerStructure_ReadWrite_23EDC2EF(param_1,param_2);

  (**(code **)(*param_2 + 0x1c))(param_2,param_1 + 0x3c,1,0x14);

  return;

}




