// /AoWEPACK.dpl : 0x5577fa50

void AoWE_TAbstractUnit_ListAbilityInfo_23EDC2EF
               (int *param_1,int *param_2,int *param_3,int *param_4)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  int *piVar4;
  uint uVar5;
  int iVar6;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_3c;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined4 local_24;
  undefined4 local_20;
  int local_1c;
  int *local_18;
  undefined4 local_14;
  int *local_10;
  int *local_c;
  int *local_8;
  
                    /* 0x7fa50  1061  AoWE.TAbstractUnit.ListAbilityInfo@23EDC2EF */
  puStack_34 = &stack0xfffffffc;
  local_24 = 0;
  local_1c = 0;
  local_20 = 0;
  puStack_38 = &LAB_5577fb88;
  uStack_3c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_3c;
  local_10 = param_3;
  local_c = param_2;
  local_8 = param_1;
  (**(code **)(*param_2 + 0x40))();
  if (param_4 != (int *)0x0) {
    (**(code **)(*param_4 + 0x40))();
  }
  iVar3 = (**(code **)(*local_8 + 0x158))();
  if (-1 < iVar3 + -1) {
    iVar6 = 0;
    do {
      cVar2 = (**(code **)(*local_8 + 0x14c))(local_8,iVar6);
      if (cVar2 != '\0') {
        local_14 = (**(code **)(*local_8 + 0x154))(local_8,iVar6);
        local_18 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                    (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),iVar6);
        (**(code **)(*local_18 + 0x58))(local_18,local_14,&local_1c);
        if (local_1c != 0) {
          (**(code **)(*local_c + 0x38))(local_c,local_1c,iVar6);
        }
        if (local_10 != (int *)0x0) {
          piVar4 = (int *)AoWE_TAbilityControl_GetAbility_23EDC2EF
                                    (*(undefined4 *)(AoWE_AoWHSSet_678301D7 + 0x80),iVar6);
          uVar5 = (**(code **)(*piVar4 + 0x54))();
          AoWE_TranslateRStr_435799AA
                    (*(undefined4 *)(&AoWE_TAbilityTypeRStr_2685A29F + (uVar5 & 0x7f) * 4),&local_24
                    );
          (**(code **)(*local_10 + 0x38))(local_10,local_24,iVar6);
        }
        (**(code **)(*local_18 + 0x8c))(local_18,local_14,&local_20);
        if (param_4 != (int *)0x0) {
          (**(code **)(*param_4 + 0x38))(param_4,local_20,iVar6);
        }
      }
      iVar6 = iVar6 + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  puVar1 = puStack_34;
  *in_FS_OFFSET = uStack_3c;
  puStack_34 = &LAB_5577fb8f;
  puStack_38 = (undefined1 *)0x5577fb87;
  System__LStrArrayClr_51F89FF7(&local_24,3,puVar1);
  return;
}

