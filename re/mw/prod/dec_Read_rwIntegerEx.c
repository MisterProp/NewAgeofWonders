
void Engine_TEReadStorageStream_rwIntegerEx_23EDC2EF
               (int param_1,undefined4 *param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  
                    /* 0x11034  599  Engine.TEReadStorageStream.rwIntegerEx@23EDC2EF */
  iVar1 = Engine_TPropertyTable_FindOffset_23EDC2EF(*(undefined4 *)(param_1 + 0x10),param_4);
  if (iVar1 == -1) {
    *param_2 = param_3;
  }
  else {
    iVar2 = Classes_TStream_GetPosition_23EDC2EF(*(undefined4 *)(param_1 + 8));
    if (iVar2 != *(int *)(param_1 + 0x28) + iVar1) {
      Classes_TStream_SetPosition_23EDC2EF
                (*(undefined4 *)(param_1 + 8),*(int *)(param_1 + 0x28) + iVar1);
    }
    (**(code **)(**(int **)(param_1 + 8) + 4))(*(int **)(param_1 + 8),param_2,4);
  }
  return;
}

