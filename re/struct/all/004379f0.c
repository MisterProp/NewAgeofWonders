
undefined4 FUN_004379f0(int param_1)

{
  undefined4 uVar1;
  
  uVar1 = (**(code **)(**(int **)(param_1 + 0xac) + 0x78))();
  if ((char)uVar1 == '\0') {
    (**(code **)(**(int **)(param_1 + 0x80) + 0x6c))(*(int **)(param_1 + 0x80),0);
  }
  return uVar1;
}

