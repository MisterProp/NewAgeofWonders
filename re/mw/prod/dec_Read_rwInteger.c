
void Engine_TEReadStorageStream_rwInteger_23EDC2EF
               (int param_1,undefined4 *param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  
                    /* 0x10ef4  603  Engine.TEReadStorageStream.rwInteger@23EDC2EF */
  iVar1 = Engine_TPropertyTable_FindOffset_23EDC2EF(*(undefined4 *)(param_1 + 0x10),param_3);
  if (iVar1 == -1) {
    *param_2 = 0;
  }
  else {
    iVar2 = Classes_TStream_GetPosition_23EDC2EF(*(undefined4 *)(param_1 + 8));
    iVar1 = *(int *)(param_1 + 0x28) + iVar1;
    if (iVar2 != iVar1) {
      Classes_TStream_SetPosition_23EDC2EF(*(undefined4 *)(param_1 + 8),iVar1);
    }
    (**(code **)(**(int **)(param_1 + 8) + 4))(*(int **)(param_1 + 8),param_2,4);
  }
  return;
}

