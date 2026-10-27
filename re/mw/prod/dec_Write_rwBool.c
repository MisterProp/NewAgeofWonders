
void Engine_TEWriteStorageStream_rwBool_23EDC2EF(int param_1,char *param_2,undefined4 param_3)

{
  char cVar1;
  
                    /* 0x10650  612  Engine.TEWriteStorageStream.rwBool@23EDC2EF */
  cVar1 = Engine_TPropertyTable_PropertyExist_23EDC2EF(*(undefined4 *)(param_1 + 0x10),param_3);
  if (cVar1 == '\0') {
    if (*param_2 != '\0') {
      Engine_TPropertyTable_Add_23EDC2EF
                (*(undefined4 *)(param_1 + 0x10),param_3,
                 *(undefined4 *)(*(int *)(param_1 + 0x28) + 0xc));
    }
  }
  else {
    Engine_TEWriteStorageStream_PropertyExistError_23EDC2EF(param_1,param_3);
  }
  return;
}

