

bool AoWE_TProductionControl_Produce_23EDC2EF(int *param_1,undefined4 param_2,undefined1 param_3)



{

  char cVar1;

  char cVar2;

  int *piVar3;

  int iVar4;

  int *piVar5;

  

                    /* 0x5d95c  2334  AoWE.TProductionControl.Produce@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x7c))();

  AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar1);

  cVar1 = AoWE_TPlayer_GetBusy_23EDC2EF();

  if (cVar1 == '\0') {

    piVar3 = (int *)(**(code **)(*param_1 + 0x74))();

    piVar3[5] = 0;

    *(undefined1 *)(piVar3 + 7) = param_3;

    iVar4 = Engine_TEObject_Copy_23EDC2EF(param_2,AoWE_AoWEngine_7E705837);

    piVar3[6] = iVar4;

    cVar2 = (**(code **)(*param_1 + 0x7c))();

    piVar5 = (int *)NetworkE_TTokenManager_GetTokenControl_23EDC2EF

                              (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x23c),(int)cVar2);

    (**(code **)(*piVar5 + 0x10))(piVar5,piVar3);

    (**(code **)(*piVar3 + 0x2c))();

  }

  return cVar1 != '\0';

}




