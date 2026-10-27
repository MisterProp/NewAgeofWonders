
void Engine_TEReadStorageStream_rwEObject_23EDC2EF(int param_1,int *param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_24;
  undefined1 *puStack_20;
  undefined1 *puStack_1c;
  
                    /* 0x116b0  585  Engine.TEReadStorageStream.rwEObject@23EDC2EF */
  uVar1 = *(undefined4 *)(param_1 + 0x28);
  puStack_1c = (undefined1 *)0x555116d4;
  iVar2 = Engine_TPropertyTable_FindOffset_23EDC2EF(*(undefined4 *)(param_1 + 0x10),param_3);
  puStack_1c = (undefined1 *)0x555116e1;
  Engine_TPropertyTable_BackupSettings_23EDC2EF(*(undefined4 *)(param_1 + 0x10));
  if (iVar2 != -1) {
    puStack_1c = (undefined1 *)0x555116f1;
    iVar3 = Classes_TStream_GetPosition_23EDC2EF(*(undefined4 *)(param_1 + 8));
    iVar2 = *(int *)(param_1 + 0x28) + iVar2;
    if (iVar3 != iVar2) {
      puStack_1c = (undefined1 *)0x5551170a;
      Classes_TStream_SetPosition_23EDC2EF(*(undefined4 *)(param_1 + 8),iVar2);
    }
    puStack_1c = (undefined1 *)0x5551171b;
    Engine_TPropertyTable_LoadFromStream_23EDC2EF
              (*(undefined4 *)(param_1 + 0x10),*(undefined4 *)(param_1 + 8));
    puStack_1c = (undefined1 *)0x55511726;
    uVar4 = Classes_TStream_GetPosition_23EDC2EF(*(undefined4 *)(param_1 + 8));
    *(undefined4 *)(param_1 + 0x28) = uVar4;
  }
  puStack_20 = &LAB_55511762;
  uStack_24 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_24;
  puStack_1c = &stack0xfffffffc;
  (**(code **)(*param_2 + 0x1c))(param_2,param_1);
  Engine_TPropertyTable_RestoreSettings_23EDC2EF(*(undefined4 *)(param_1 + 0x10));
  *(undefined4 *)(param_1 + 0x28) = uVar1;
  *in_FS_OFFSET = uStack_24;
  return;
}

