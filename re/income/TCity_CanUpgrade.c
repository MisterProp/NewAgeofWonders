

void City_TCity_CanUpgrade_23EDC2EF(int param_1,char param_2,undefined4 param_3)



{

  undefined1 *puVar1;

  int iVar2;

  int iVar3;

  int iVar4;

  int iVar5;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_2c;

  undefined1 *puStack_28;

  undefined1 *puStack_24;

  undefined4 local_14;

  undefined4 local_10;

  uint local_c;

  undefined4 local_8;

  

                    /* 0xaabd4  6904  City.TCity.CanUpgrade@23EDC2EF */

  puStack_24 = &stack0xfffffffc;

  local_10 = 0;

  local_14 = 0;

  puStack_28 = &LAB_557aac8d;

  uStack_2c = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_2c;

  local_c = (uint)*(byte *)(param_1 + 0x4d);

  puVar1 = &stack0xfffffffc;

  local_8 = param_3;

  if ((param_2 != '\0') &&

     (iVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0x50) + 0x14) + 0x54))(),

     puVar1 = puStack_24, -1 < iVar2 + -1)) {

    iVar5 = 0;

    do {

      iVar3 = AoWE_TProductionQueue_GetItem_23EDC2EF

                        (*(undefined4 *)(*(int *)(param_1 + 0x50) + 0x14),iVar5);

      iVar3 = *(int *)(iVar3 + 8);

      iVar4 = City_CityActionToProductionID_CFDECB0A(CONCAT31((int3)((uint)iVar3 >> 8),2));

      if (iVar3 == iVar4) {

        local_c = local_c + 1;

      }

      iVar5 = iVar5 + 1;

      iVar2 = iVar2 + -1;

      puVar1 = puStack_24;

    } while (iVar2 != 0);

  }

  puStack_24 = puVar1;

  iVar2 = City_TCity_GetSize_23EDC2EF(param_1);

  if (iVar2 <= (int)local_c) {

    System_LoadResString_02614E1B(PTR_AoWE_MaxUpgradeLevelReachedRStr_04ECFCC5_558e9224,&local_14);

    AoWE_TranslateRStr_435799AA(local_14,&local_10);

    System__LStrAsg_51F89FF7(local_8,local_10);

  }

  puVar1 = puStack_24;

  *in_FS_OFFSET = uStack_2c;

  puStack_24 = &LAB_557aac94;

  puStack_28 = (undefined1 *)0x557aac8c;

  System__LStrArrayClr_51F89FF7(&local_14,2,puVar1);

  return;

}




