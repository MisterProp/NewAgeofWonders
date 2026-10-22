
void FUN_00450e88(int param_1,undefined4 param_2,undefined1 param_3,int param_4,int param_5,
                 undefined1 param_6)

{
  char cVar1;
  
  cVar1 = FUN_00450de8(param_1,param_5,param_4);
  if (cVar1 == '\0') {
    (**(code **)(**(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc) + 0x1c))
              (*(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc),param_3,param_6,0xffffffff,
               0xffffffff);
  }
  else {
    (**(code **)(**(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc) + 0x1c))
              (*(int **)(*(int *)AoWE_AoWHSMap_6B957A58_exref + 0xc),param_3,param_6,
               (param_4 + -0xc) - *(int *)(*(int *)(param_1 + 0x44) + 0x88),
               (param_5 + -0xc) - *(int *)(*(int *)(param_1 + 0x44) + 0x84));
  }
  return;
}

