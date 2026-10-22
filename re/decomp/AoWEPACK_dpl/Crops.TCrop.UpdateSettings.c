
void Crops_TCrop_UpdateSettings_23EDC2EF(int *param_1)

{
  char cVar1;
  int iVar2;
  
                    /* 0xa5cc8  6762  Crops.TCrop.UpdateSettings@23EDC2EF */
  iVar2 = (**(code **)(*param_1 + 300))(param_1,(int)(char)param_1[6]);
  param_1[5] = iVar2;
  cVar1 = ImageLib_TCustomImageLibrary_ImageExist_23EDC2EF(*(undefined4 *)(param_1[2] + 0x20),iVar2)
  ;
  if (cVar1 == '\0') {
    (**(code **)(*param_1 + -4))(param_1,1);
  }
  return;
}

