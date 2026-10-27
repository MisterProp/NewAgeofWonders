

undefined4 AoWE_TAbstractUnit_GetManaUpkeep_23EDC2EF(int *param_1)



{

  char cVar1;

  int iVar2;

  undefined4 uVar3;

  

                    /* 0x7fc90  1051  AoWE.TAbstractUnit.GetManaUpkeep@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x148))(param_1,0x9b);

  if (cVar1 == '\0') {

    uVar3 = 0;

  }

  else {

    iVar2 = AoWE_TAbilityOwner_GetAbilityData_23EDC2EF(param_1,0x9b);

    if (*(int *)(iVar2 + 0x10) == 0) {

      return 0;

    }

    uVar3 = *(undefined4 *)(*(int *)(iVar2 + 0x10) + 0x10);

  }

  return uVar3;

}




