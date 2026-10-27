

undefined4 PowerNode_TPowerNode_GetPower_23EDC2EF(int *param_1)



{

  char cVar1;

  

                    /* 0xd03c8  8165  PowerNode.TPowerNode.GetPower@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x150))();

  if (cVar1 == '\0') {

    cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1);

    if (cVar1 == '\0') {

      return 10;

    }

  }

  return 0;

}




