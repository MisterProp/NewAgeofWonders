
void ActivityDoubleClickCity(undefined4 param_1,int *param_2)

{
  undefined4 extraout_EDX;
  
  if ((char)param_2[3] != '\x03') {
    FUN_0043c2ec(param_1);
    FUN_0043c43c(param_1,CONCAT31((int3)((uint)extraout_EDX >> 8),(char)param_2[3]) + 1);
    (**(code **)(*param_2 + 0xac))(param_2,1);
    CityBtn1Click(param_1,0);
  }
  return;
}

