
void ActivityChange(undefined4 param_1,int *param_2)

{
  FUN_00455dfc(*(undefined4 *)PTR_DAT_0045a420,1);
  if ((char)param_2[0x2c] != '\0') {
    FUN_0043f4a4(param_1);
    FUN_0043ffa8(param_1,param_2[3]);
    (**(code **)(*param_2 + 0xac))(param_2,1);
  }
  return;
}

