
void Engine_TEWriteStorageStream_rwEObject_23EDC2EF(int param_1,int *param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  char cVar2;
  undefined4 uVar3;
  undefined4 *in_FS_OFFSET;
  undefined *puStack_240;
  undefined1 *puStack_23c;
  undefined1 *puStack_238;
  undefined4 uStack_234;
  undefined1 *puStack_230;
  undefined1 *puStack_22c;
  undefined1 *local_21c;
  undefined1 local_218 [256];
  undefined1 local_118 [256];
  undefined1 *local_18;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  int local_8;
  
                    /* 0x10acc  608  Engine.TEWriteStorageStream.rwEObject@23EDC2EF */
  puStack_22c = &stack0xfffffffc;
  local_14 = 0;
  local_18 = (undefined1 *)0x0;
  local_21c = (undefined1 *)0x0;
  puStack_230 = &LAB_55510caf;
  uStack_234 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_234;
  puStack_238 = (undefined1 *)0x55510b08;
  local_8 = param_1;
  cVar2 = Engine_TPropertyTable_PropertyExist_23EDC2EF(*(undefined4 *)(param_1 + 0x10),param_3);
  if (cVar2 == '\0') {
    local_c = *(int *)(local_8 + 0x28);
    local_10 = *(undefined4 *)(local_c + 0xc);
    puStack_238 = (undefined1 *)0x55510b2b;
    uVar3 = System_TObject_Create_23EDC2EF
                      (&Engine__TEBufferStream_B27C7423,CONCAT31((int3)((uint)local_10 >> 8),1));
    *(undefined4 *)(local_8 + 0x28) = uVar3;
    puStack_238 = (undefined1 *)0x55510b3c;
    Engine_TPropertyTable_BackupSettings_23EDC2EF(*(undefined4 *)(local_8 + 0x10));
    puStack_23c = &LAB_55510bac;
    puStack_240 = (undefined *)*in_FS_OFFSET;
    *in_FS_OFFSET = &puStack_240;
    puStack_238 = &stack0xfffffffc;
    (**(code **)(*param_2 + 0x1c))(param_2,local_8);
    Engine_TPropertyTable_SaveToStream_23EDC2EF(*(undefined4 *)(local_8 + 0x10),local_c);
    FMemStr_TFastMemoryStream_SaveToStream_23EDC2EF(*(undefined4 *)(local_8 + 0x28),local_c);
    Engine_TPropertyTable_RestoreSettings_23EDC2EF(*(undefined4 *)(local_8 + 0x10));
    System_TObject_Free_23EDC2EF(*(undefined4 *)(local_8 + 0x28));
    *(int *)(local_8 + 0x28) = local_c;
    Engine_TPropertyTable_Add_23EDC2EF(*(undefined4 *)(local_8 + 0x10),param_3,local_10);
    *in_FS_OFFSET = puStack_240;
  }
  else {
    puStack_238 = (undefined1 *)0x55510c11;
    System__PStrCpy_51F89FF7(local_118,&DAT_55510cc0);
    puStack_238 = (undefined1 *)0x55510c1e;
    System_TObject_ClassName_23EDC2EF(*param_2,local_218);
    puStack_238 = (undefined1 *)0x55510c2f;
    System__PStrCat_51F89FF7(local_118,local_218);
    puStack_238 = (undefined1 *)0x55510c3f;
    System__PStrCat_51F89FF7(local_118,&DAT_55510cdc);
    puStack_238 = (undefined1 *)0x55510c4d;
    System__LStrFromString_EF219F52(&local_18,local_118);
    puStack_238 = local_18;
    puStack_23c = (undefined1 *)0x55510c5d;
    SysUtils_IntToStr_0F6FDFF6(param_3,&local_21c);
    puStack_23c = local_21c;
    puStack_240 = &DAT_55510ce8;
    System__LStrCatN_51F89FF7(&local_14,3);
    SysUtils_Exception_Create_23EDC2EF(&Engine__EDoublePrioriyID_01884AAF,1,local_14);
    System__RaiseExcept_51F89FF7();
  }
  puVar1 = puStack_22c;
  *in_FS_OFFSET = uStack_234;
  puStack_22c = &LAB_55510cb6;
  puStack_230 = (undefined1 *)0x55510ca1;
  System__LStrClr_40929B27(&local_21c,uStack_234,puVar1);
  puStack_230 = (undefined1 *)0x55510cae;
  System__LStrArrayClr_51F89FF7(&local_18,2);
  return;
}

