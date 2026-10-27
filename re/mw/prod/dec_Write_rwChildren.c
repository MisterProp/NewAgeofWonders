
void Engine_TEWriteStorageStream_rwChildren_23EDC2EF(int *param_1,int *param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined4 *in_FS_OFFSET;
  undefined *puVar5;
  undefined4 uVar6;
  char *pcVar7;
  undefined4 uStack_3c;
  undefined1 *puStack_38;
  undefined1 *puStack_34;
  undefined4 local_24;
  undefined4 local_20;
  int local_1c;
  int local_18;
  int *local_14;
  undefined4 local_10;
  int local_c;
  undefined4 local_8;
  
                    /* 0x10940  609  Engine.TEWriteStorageStream.rwChildren@23EDC2EF */
  puStack_34 = &stack0xfffffffc;
  local_20 = 0;
  local_24 = 0;
  puStack_38 = &LAB_55510a8e;
  uStack_3c = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_3c;
  local_8 = param_3;
  cVar2 = Engine_TPropertyTable_PropertyExist_23EDC2EF(param_1[4],param_3);
  if (cVar2 == '\0') {
    local_c = param_1[10];
    local_10 = *(undefined4 *)(local_c + 0xc);
    iVar3 = System_TObject_Create_23EDC2EF
                      (&Engine__TEBufferStream_B27C7423,CONCAT31((int3)((uint)local_10 >> 8),1));
    param_1[10] = iVar3;
    Engine_TPropertyTable_BackupSettings_23EDC2EF(param_1[4]);
    iVar3 = (**(code **)(*param_2 + 0x54))();
    if (-1 < iVar3 + -1) {
      local_18 = 0;
      local_1c = iVar3;
      do {
        local_14 = (int *)(**(code **)(*param_2 + 0x4c))(param_2,local_18);
        if ((local_14 != (int *)0x0) && (param_2 == (int *)local_14[1])) {
          uVar4 = (**(code **)(*local_14 + 4))();
          if ((uVar4 & 2) == 0) {
            uVar4 = (**(code **)(*local_14 + 4))();
            if ((uVar4 & 1) != 0) {
              (**(code **)(*param_1 + 8))(param_1,&local_14,param_2,local_18);
            }
          }
        }
        local_18 = local_18 + 1;
        local_1c = local_1c + -1;
      } while (local_1c != 0);
    }
    Engine_TPropertyTable_SaveToStream_23EDC2EF(param_1[4],local_c);
    FMemStr_TFastMemoryStream_SaveToStream_23EDC2EF(param_1[10],local_c);
    System_TObject_Free_23EDC2EF(param_1[10]);
    Engine_TPropertyTable_RestoreSettings_23EDC2EF(param_1[4]);
    param_1[10] = local_c;
    Engine_TPropertyTable_Add_23EDC2EF(param_1[4],local_8,local_10);
  }
  else {
    pcVar7 = "Double priorityID found (";
    SysUtils_IntToStr_0F6FDFF6(local_8,&local_24);
    puVar5 = &DAT_55510ac8;
    uVar6 = local_24;
    System__LStrCatN_51F89FF7(&local_20,3);
    SysUtils_Exception_Create_23EDC2EF
              (&Engine__EDoublePrioriyID_01884AAF,1,local_20,puVar5,uVar6,pcVar7);
    System__RaiseExcept_51F89FF7();
  }
  puVar1 = puStack_34;
  *in_FS_OFFSET = uStack_3c;
  puStack_34 = &LAB_55510a95;
  puStack_38 = (undefined1 *)0x55510a8d;
  System__LStrArrayClr_51F89FF7(&local_24,2,puVar1);
  return;
}

