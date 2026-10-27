
void Engine_TEReadStorageStream_rwChildren_23EDC2EF(int *param_1,int *param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iStack_18;
  int *piStack_14;
  
                    /* 0x115a4  586  Engine.TEReadStorageStream.rwChildren@23EDC2EF */
  iVar3 = Engine_TPropertyTable_FindOffset_23EDC2EF(param_1[4],param_3);
  if (iVar3 != -1) {
    iVar1 = param_1[10];
    Engine_TPropertyTable_BackupSettings_23EDC2EF(param_1[4]);
    iVar4 = Classes_TStream_GetPosition_23EDC2EF(param_1[2]);
    if (iVar4 != param_1[10] + iVar3) {
      Classes_TStream_SetPosition_23EDC2EF(param_1[2],param_1[10] + iVar3);
    }
    Engine_TPropertyTable_LoadFromStream_23EDC2EF(param_1[4],param_1[2]);
    iVar3 = Classes_TStream_GetPosition_23EDC2EF(param_1[2]);
    param_1[10] = iVar3;
    uVar5 = (**(code **)(*param_2 + 4))();
    if ((uVar5 & 8) == 0) {
      iStack_18 = *(int *)(param_1[4] + 0x24);
      iVar3 = *(int *)(param_1[4] + 0xc) + -1;
      if (iStack_18 <= iVar3) {
        iVar3 = (iVar3 - iStack_18) + 1;
        do {
          (**(code **)(*param_1 + 8))
                    (param_1,&piStack_14,param_2,
                     *(undefined4 *)(*(int *)(param_1[4] + 4) + iStack_18 * 8));
          iStack_18 = iStack_18 + 1;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
    }
    else {
      iStack_18 = *(int *)(param_1[4] + 0x24);
      iVar3 = *(int *)(param_1[4] + 0xc) + -1;
      if (iStack_18 <= iVar3) {
        iVar3 = (iVar3 - iStack_18) + 1;
        do {
          uVar2 = *(undefined4 *)(*(int *)(param_1[4] + 4) + iStack_18 * 8);
          (**(code **)(*param_1 + 8))(param_1,&piStack_14,param_2,uVar2);
          if (piStack_14 != (int *)0x0) {
            (**(code **)(*piStack_14 + 0x48))(piStack_14,param_2,uVar2);
          }
          iStack_18 = iStack_18 + 1;
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
      }
    }
    Engine_TPropertyTable_RestoreSettings_23EDC2EF(param_1[4]);
    param_1[10] = iVar1;
  }
  return;
}

