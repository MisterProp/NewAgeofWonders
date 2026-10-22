
void FUN_00412114(int param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  
  iVar2 = (**(code **)(**(int **)(*(int *)(param_1 + 0x5c) + 0x114) + 0x14))();
  if (param_5 < iVar2) {
    piVar1 = *(int **)(*(int *)(param_1 + 0x5c) + 0x114);
    uVar3 = (**(code **)(*piVar1 + 0x18))(piVar1,param_5);
    iVar2 = AoWSetup_TRemotePlayerList_IndexOfRemoteID_23EDC2EF
                      (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x38),uVar3);
    if (iVar2 != -1) {
      iVar4 = AoWSetup_TRemotePlayerList_GetPlayers_23EDC2EF
                        (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x38),iVar2);
      if (*(char *)(iVar4 + 0x11) == '\x01') {
        ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a210 + 0x54),0xff);
        ImageLib_TImageLibraryFont_SetText_23EDC2EF
                  (*(undefined4 *)(*(int *)PTR_DAT_0045a210 + 0x54),*(int *)(param_3 + 8) + -8,
                   *(int *)(param_3 + 4) + 4,param_4,&LAB_00412218);
      }
      else {
        iVar2 = AoWSetup_TRemotePlayerList_GetPlayers_23EDC2EF
                          (*(undefined4 *)(*(int *)(*(int *)(param_1 + 0x33c) + 0x28) + 0x38),iVar2)
        ;
        if (*(char *)(iVar2 + 0x11) == '\x02') {
          ImageLib_TImageLibraryFont_SetColor_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_DAT_0045a210 + 0x54),0x8000);
          ImageLib_TImageLibraryFont_SetText_23EDC2EF
                    (*(undefined4 *)(*(int *)PTR_DAT_0045a210 + 0x54),*(int *)(param_3 + 8) + -8,
                     *(int *)(param_3 + 4) + 4,param_4,&LAB_00412218);
        }
      }
    }
  }
  return;
}

