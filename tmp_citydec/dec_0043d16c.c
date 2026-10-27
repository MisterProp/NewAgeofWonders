
void ActivityChangeCity(undefined4 param_1,int *param_2)

{
  undefined4 extraout_EDX;
  
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,1);
  if ((char)param_2[0x2c] != '\0') {
    FUN_0043c2ec(param_1);
    FUN_0043c43c(param_1,CONCAT31((int3)((uint)extraout_EDX >> 8),(char)param_2[3]) + 1);
    (**(code **)(*param_2 + 0xac))(param_2,1);
  }
  return;
}

