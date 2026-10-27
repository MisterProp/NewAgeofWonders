
void Engine_TEReadStorageStream_rwBool_23EDC2EF(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
                    /* 0x11384  589  Engine.TEReadStorageStream.rwBool@23EDC2EF */
  iVar1 = Engine_TPropertyTable_FindOffset_23EDC2EF(*(undefined4 *)(param_1 + 0x10),param_3);
  *(bool *)param_2 = iVar1 != -1;
  return;
}

