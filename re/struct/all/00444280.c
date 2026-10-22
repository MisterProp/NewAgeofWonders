
void FUN_00444280(int param_1,int param_2)

{
  int *piVar1;
  undefined1 *puVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  puStack_1c = &stack0xfffffffc;
  local_c = 0;
  local_10 = 0;
  puStack_20 = &LAB_0044448f;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  local_8 = param_2;
  FUN_004425d8(param_1);
  System_LoadResString_02614E1B(AoWE_ItemBagRStr_8276FCC3_exref,&local_10);
  AoWE_TranslateRStr_435799AA(local_10,&local_c);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x50),local_c);
  *(int *)(param_1 + 0x1b0) = local_8;
  (**(code **)(**(int **)(param_1 + 0x170) + 0x6c))(*(int **)(param_1 + 0x170),1);
  (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
  AoWLabel_TAOWLabel_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 100),0);
  uVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xb4),0x23);
  AoWArnoLine_TAoWArnoLine_SetITop_23EDC2EF(*(undefined4 *)(param_1 + 0x60),uVar3);
  uVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xb4),0x24);
  AoWArnoLine_TAoWArnoLine_SetIMiddle_23EDC2EF(*(undefined4 *)(param_1 + 0x60),uVar3);
  uVar3 = ImageLib_TCustomImageLibrary_Get_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_DAT_0045a4a0 + 0xb4),0x25);
  AoWArnoLine_TAoWArnoLine_SetIBottom_23EDC2EF(*(undefined4 *)(param_1 + 0x60),uVar3);
  iVar4 = AoWE_TAoWMapField_GetArmyHS_23EDC2EF(*(undefined4 *)(local_8 + 4));
  if ((iVar4 == 0) ||
     (*(char *)(*(int *)(iVar4 + 0x1c) + 0x12) !=
      *(char *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xa5))) {
    (**(code **)(**(int **)(param_1 + 0x180) + 0x6c))(*(int **)(param_1 + 0x180),1);
    (**(code **)(**(int **)(param_1 + 0x178) + 0x6c))(*(int **)(param_1 + 0x178),0);
    (**(code **)(**(int **)(param_1 + 0x174) + 0x6c))(*(int **)(param_1 + 0x174),0);
    (**(code **)(**(int **)(param_1 + 0x17c) + 0x6c))(*(int **)(param_1 + 0x17c),0);
    (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),0);
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x180) + 0x6c))(*(int **)(param_1 + 0x180),0);
    iVar4 = AoWE_TArmy_Hero_23EDC2EF(*(undefined4 *)(iVar4 + 0x1c),0xff,*AoWE_htAll_E90514D2_exref);
    (**(code **)(**(int **)(param_1 + 0x58) + 0x6c))(*(int **)(param_1 + 0x58),iVar4 != 0);
    System_LoadResString_02614E1B(AoWE_PickupRStr_04ECFCC5_exref,&local_10);
    AoWE_TranslateRStr_435799AA(local_10,&local_c);
    AoWButton_TAOWBaseButton_SetGText_23EDC2EF(*(undefined4 *)(param_1 + 0x58),local_c);
    (**(code **)(**(int **)(param_1 + 0x178) + 0x6c))(*(int **)(param_1 + 0x178),1);
    (**(code **)(**(int **)(*(int *)(param_1 + 0x178) + 0x114) + 0x40))();
    (**(code **)(**(int **)(param_1 + 0x17c) + 0x6c))(*(int **)(param_1 + 0x17c),1);
    iVar4 = (**(code **)(**(int **)(local_8 + 0xc) + 0x54))();
    if (-1 < iVar4 + -1) {
      do {
        piVar1 = *(int **)(*(int *)(param_1 + 0x178) + 0x114);
        (**(code **)(*piVar1 + 0x34))(piVar1,0);
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
    (**(code **)(**(int **)(param_1 + 0x174) + 0x6c))(*(int **)(param_1 + 0x174),1);
  }
  puVar2 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_00444496;
  puStack_20 = (undefined1 *)0x44448e;
  System__LStrArrayClr_51F89FF7(&local_10,2,puVar2);
  return;
}

