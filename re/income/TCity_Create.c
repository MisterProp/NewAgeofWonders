

void City_TCity_Create_23EDC2EF(int param_1,char param_2)



{

  undefined1 *puVar1;

  int iVar2;

  undefined4 uVar3;

  undefined1 extraout_DL;

  undefined1 uVar4;

  undefined4 *in_FS_OFFSET;

  undefined4 uStack_34;

  undefined1 *puStack_30;

  undefined1 *puStack_2c;

  undefined4 local_10;

  undefined4 local_c;

  undefined4 uStack_8;

  

                    /* 0xa952c  6943  City.TCity.Create@23EDC2EF */

  uStack_8 = 0;

  local_c = 0;

  local_10 = 0;

  uVar4 = 0;

  if (param_2 != '\0') {

    puStack_2c = (undefined1 *)0x557a9543;

    param_1 = System__ClassCreate_51F89FF7();

    uVar4 = extraout_DL;

  }

  uStack_8 = CONCAT13(uVar4,(undefined3)uStack_8);

  puStack_30 = &LAB_557a95ea;

  uStack_34 = *in_FS_OFFSET;

  *in_FS_OFFSET = &uStack_34;

  puStack_2c = &stack0xfffffffc;

  Crops_TPlayerCropStructure_Create_23EDC2EF(param_1,0);

  iVar2 = AoWE_TProductionControl_Create_23EDC2EF(&City__TCityProductionControl_E7756B2F,1,0);

  *(int *)(param_1 + 0x50) = iVar2;

  *(int *)(iVar2 + 0x28) = param_1;

  *(undefined1 *)(param_1 + 0x74) = 3;

  *(undefined1 *)(param_1 + 0x45) = 0xff;

  System_LoadResString_02614E1B(PTR_AoWE_NonameRStr_04ECFCC5_558e9388,&local_10);

  AoWE_TranslateRStr_435799AA(local_10,&local_c);

  System__LStrAsg_51F89FF7(param_1 + 0x48,local_c);

  *(undefined1 *)(param_1 + 0x4d) = 1;

  iVar2 = Engine_TEObject_Create_23EDC2EF(&City__TCityOverlayHS_7F5D3414,1,0);

  *(int *)(param_1 + 0x60) = iVar2;

  *(int *)(iVar2 + 0xc) = param_1;

  uVar3 = Engine_TENode_Create_23EDC2EF(&AoWE__TGlobalEnchantmentList_AD316CC6,1,0);

  puVar1 = puStack_2c;

  *(undefined4 *)(param_1 + 0x58) = uVar3;

  *(undefined1 *)(param_1 + 100) = 0;

  *in_FS_OFFSET = uStack_34;

  puStack_2c = &LAB_557a95f1;

  puStack_30 = (undefined1 *)0x557a95e9;

  System__LStrArrayClr_51F89FF7(&local_10,2,puVar1);

  return;

}




