

bool AoWE_TArmy_AddUnits_23EDC2EF(int *param_1,int *param_2)



{

  char cVar1;

  int iVar2;

  undefined4 uVar3;

  

                    /* 0x8e500  372  AoWE.TArmy.AddUnits@23EDC2EF */

  cVar1 = AoWE_TArmy_CanAddUnits_23EDC2EF(param_1,param_2);

  if (cVar1 == '\0') {

    Engine_TEChangeNotifyNode_BeginUpdate_23EDC2EF(param_1);

    iVar2 = (**(code **)(*param_2 + 0x54))();

    iVar2 = iVar2 + -1;

    if (-1 < iVar2) {

      do {

        uVar3 = AoWE_TUnitList_GetUnit_23EDC2EF(param_2,iVar2);

        (**(code **)(*param_1 + 0xac))(param_1,uVar3);

        iVar2 = iVar2 + -1;

      } while (iVar2 != -1);

    }

    Engine_TEChangeNotifyNode_EndUpdate_23EDC2EF(param_1);

  }

  return cVar1 == '\0';

}




