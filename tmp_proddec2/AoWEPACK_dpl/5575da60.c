// /AoWEPACK.dpl : 5575da60

undefined4 AoWE_TProductionControl_DeleteProductions_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  
                    /* 0x5da60  2332  AoWE.TProductionControl.DeleteProductions@23EDC2EF */
  iVar2 = (**(code **)(*(int *)param_1[5] + 0x54))();
  if (iVar2 != 0) {
    cVar1 = (**(code **)(*param_1 + 0x7c))();
    AoWE_TPlayerList_GetPlayers_23EDC2EF(*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x140),(int)cVar1)
    ;
    cVar1 = AoWE_TPlayer_GetBusy_23EDC2EF();
    if (cVar1 == '\0') {
      piVar3 = (int *)(**(code **)(*param_1 + 0x74))();
      piVar3[5] = 2;
      cVar1 = (**(code **)(*param_1 + 0x7c))();
      piVar4 = (int *)NetworkE_TTokenManager_GetTokenControl_23EDC2EF
                                (*(undefined4 *)(AoWE_AoWHSMap_6B957A58 + 0x23c),(int)cVar1);
      (**(code **)(*piVar4 + 0x10))(piVar4,piVar3);
      (**(code **)(*piVar3 + 0x2c))();
      return 1;
    }
  }
  return 0;
}

