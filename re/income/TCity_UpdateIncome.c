

char City_TCity_UpdateIncome_23EDC2EF(int *param_1)



{

  char cVar1;

  byte bVar2;

  int iVar3;

  undefined4 extraout_EDX;

  undefined4 extraout_EDX_00;

  undefined4 uVar4;

  

                    /* 0xabb94  6869  City.TCity.UpdateIncome@23EDC2EF */

  cVar1 = (**(code **)(*param_1 + 0x150))();

  if (((cVar1 != '\0') || (cVar1 = City_TCity_GetActiveAction_23EDC2EF(param_1), cVar1 == '\x04'))

     || (cVar1 = AoWE_TStructure_GetBuilding_23EDC2EF(param_1), cVar1 != '\0')) {

    cVar1 = Crops_TPlayerCropStructure_SetIncome_23EDC2EF(param_1,0);

    return cVar1;

  }

  iVar3 = Crops_TPlayerCropStructure_GetCropCount_23EDC2EF(param_1);

  cVar1 = City_TCity_GetSize_23EDC2EF(param_1);

  Crops_TPlayerCropStructure_SetIncome_23EDC2EF

            (param_1,CONCAT31((int3)((uint)(iVar3 * 2) >> 8),(char)(iVar3 * 2) + cVar1 * '\x05'));

  bVar2 = City_TCity_GetActiveAction_23EDC2EF(param_1);

  cVar1 = bVar2 - 2;

  uVar4 = extraout_EDX;

  if (bVar2 < 2) {

    iVar3 = param_1[0xf];

    cVar1 = System__ROUND_51F89FF7();

    cVar1 = Crops_TPlayerCropStructure_SetIncome_23EDC2EF(param_1,cVar1 + (char)iVar3);

    uVar4 = extraout_EDX_00;

  }

  if ((*(byte *)(param_1 + 0x17) & 2) == 0) {

    return cVar1;

  }

  cVar1 = Crops_TPlayerCropStructure_SetIncome_23EDC2EF

                    (param_1,CONCAT31((int3)((uint)uVar4 >> 8),(char)param_1[0xf]) * 2);

  return cVar1;

}




