
void Engine_TEWriteStorageStream_rwInteger_23EDC2EF(int param_1,int *param_2,undefined4 param_3)

{
  char cVar1;
  
                    /* 0x10268  623  Engine.TEWriteStorageStream.rwInteger@23EDC2EF */
  cVar1 = Engine_TPropertyTable_PropertyExist_23EDC2EF(*(undefined4 *)(param_1 + 0x10),param_3);
  if (cVar1 == '\0') {
    if (*param_2 != 0) {
      Engine_TPropertyTable_Add_23EDC2EF
                (*(undefined4 *)(param_1 + 0x10),param_3,
                 *(undefined4 *)(*(int *)(param_1 + 0x28) + 0xc));
      (**(code **)(**(int **)(param_1 + 0x28) + 8))(*(int **)(param_1 + 0x28),param_2,4);
    }
  }
  else {
    SysUtils_Exception_Create_23EDC2EF
              (&Engine__EDoublePrioriyID_01884AAF,1,"PropertyID already in use");
    System__RaiseExcept_51F89FF7();
  }
  return;
}

