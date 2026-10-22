
void FUN_00439adc(int param_1,int *param_2,undefined4 param_3,code *param_4,undefined4 param_5)

{
  int iVar1;
  
  if (*(char *)(*(int *)PTR_DAT_0045a5fc + 0xad) == '\0') {
    *(int **)(param_1 + 0x8c) = param_2;
    (**(code **)(*param_2 + 0x28))();
    *(code **)(param_1 + 0x90) = param_4;
    *(undefined4 *)(param_1 + 0x94) = param_5;
    AOWWinManager_TAOWWinManager_SetOverPri_23EDC2EF
              (*(undefined4 *)(*(int *)PTR_DAT_0045a420 + 0x44),0x31);
    if (*(char *)(*(int *)(param_1 + 0x74) + 0xb0) == '\0') {
      AoWE_TCombatTypeRequestEventLog_Select_23EDC2EF(param_2,1);
    }
    else {
      AoWE_TCombatTypeRequestEventLog_Select_23EDC2EF(param_2,2);
    }
    iVar1 = AoWE_TPlayerList_GetPlayers_23EDC2EF
                      (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x140),
                       (int)*(char *)(*(int *)(param_1 + 0x8c) + 0x19));
    if (*(char *)(iVar1 + 0xa7) == '\0') {
      (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),1);
      *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x50) + 0xd4) + 0x28) = 1;
    }
    else {
      (**(code **)(**(int **)(param_1 + 0x54) + 0x6c))(*(int **)(param_1 + 0x54),0);
      *(undefined1 *)(*(int *)(*(int *)(param_1 + 0x50) + 0xd4) + 0x28) = 4;
    }
    (**(code **)(**(int **)(param_1 + 0x44) + 0x34))();
    (**(code **)(**(int **)(param_1 + 0x44) + 0x6c))(*(int **)(param_1 + 0x44),1);
    AoWE_TTutorialControl_ShowMessage_23EDC2EF
              (*(undefined4 *)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0x124),0x3d,1,1);
  }
  else {
    if (*(char *)(*(int *)(param_1 + 0x74) + 0xb0) == '\0') {
      AoWE_TCombatTypeRequestEventLog_Select_23EDC2EF(param_2,1);
    }
    else {
      AoWE_TCombatTypeRequestEventLog_Select_23EDC2EF(param_2,2);
    }
    (*param_4)(param_5,param_2);
  }
  return;
}

