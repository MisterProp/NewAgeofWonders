// /AoWEPACK.dpl : 5575d7e4

void AoWE_TProductionControl_CanResearchUnit_23EDC2EF
               (int param_1,undefined4 param_2,char param_3,undefined4 param_4)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0x5d7e4  2336  AoWE.TProductionControl.CanResearchUnit@23EDC2EF */
  puStack_1c = &stack0xfffffffc;
  local_8 = 0;
  local_c = 0;
  puStack_20 = &LAB_5575d85e;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  puVar1 = &stack0xfffffffc;
  if ((param_3 != '\0') &&
     (iVar2 = AoWE_TProductionQueue_IndexOf_23EDC2EF(*(undefined4 *)(param_1 + 0x14),2,param_2,0),
     puVar1 = puStack_1c, iVar2 != -1)) {
    System_LoadResString_02614E1B(&AoWE_ActionAlreadyQueuedRStr_8276FCC3,&local_c);
    AoWE_TranslateRStr_435799AA(local_c,&local_8);
    System__LStrAsg_51F89FF7(param_4,local_8);
    puVar1 = puStack_1c;
  }
  puStack_1c = puVar1;
  puVar1 = puStack_1c;
  *in_FS_OFFSET = uStack_24;
  puStack_1c = &LAB_5575d865;
  puStack_20 = (undefined1 *)0x5575d85d;
  System__LStrArrayClr_51F89FF7(&local_c,2,puVar1);
  return;
}

