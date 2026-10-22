
void FUN_00430228(int param_1,char param_2)

{
  int iVar1;
  int iVar2;
  
  *(char *)(param_1 + 0x221) = param_2;
  (**(code **)(**(int **)(param_1 + 0x48) + 0x6c))(*(int **)(param_1 + 0x48),param_2 == '\x01');
  (**(code **)(**(int **)(param_1 + 0xb0) + 0x6c))(*(int **)(param_1 + 0xb0),param_2 == '\x01');
  (**(code **)(**(int **)(param_1 + 0x178) + 0x6c))(*(int **)(param_1 + 0x178),param_2 == '\0');
  (**(code **)(**(int **)(param_1 + 500) + 0x6c))(*(int **)(param_1 + 500),param_2 == '\0');
  if (param_2 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))(*(int **)(param_1 + 0x138),0);
    (**(code **)(**(int **)(param_1 + 0x134) + 0x6c))(*(int **)(param_1 + 0x134),0);
  }
  else if (param_2 == '\x01') {
    iVar1 = AoWE_TSpellList_GetCount_23EDC2EF(*(undefined4 *)(param_1 + 0x224));
    iVar2 = *(int *)(param_1 + 0x228) + 1;
    (**(code **)(**(int **)(param_1 + 0x138) + 0x6c))
              (*(int **)(param_1 + 0x138),
               CONCAT31((int3)((uint)(iVar2 * 8) >> 8),0 < iVar1 + iVar2 * -8));
    (**(code **)(**(int **)(param_1 + 0x134) + 0x6c))
              (*(int **)(param_1 + 0x134),*(int *)(param_1 + 0x228) != 0);
    return;
  }
  return;
}

