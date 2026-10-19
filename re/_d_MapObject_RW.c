
void HSEngine_TMapObject_ReadWrite_23EDC2EF(int *param_1,int *param_2)

{
  undefined1 *puVar1;
  uint uVar2;
  undefined4 *in_FS_OFFSET;
  undefined4 uStack_228;
  undefined1 *puStack_224;
  undefined1 *puStack_220;
  undefined4 local_210;
  undefined1 local_20c [256];
  undefined1 local_10c [256];
  undefined4 local_c;
  undefined4 local_8;
  
                    /* 0x9824  280  HSEngine.TMapObject.ReadWrite@23EDC2EF */
  puStack_220 = &stack0xfffffffc;
  local_c = 0;
  local_210 = 0;
  puStack_224 = &LAB_5560995e;
  uStack_228 = *in_FS_OFFSET;
  *in_FS_OFFSET = &uStack_228;
  uVar2 = (**(code **)*param_2)();
  if ((uVar2 & 0x44) == 0) {
    uVar2 = (**(code **)*param_2)();
    if ((uVar2 & 1) == 0) {
      local_8 = HSEngine_TMapObject_GetResourceID_23EDC2EF(param_1);
      (**(code **)(*param_2 + 0x2c))(param_2,&local_8,1);
    }
    else {
      HSEngine_TMapObject_SetResourceLinked_23EDC2EF(param_1,0);
      (**(code **)(*param_2 + 0x2c))(param_2,param_1 + 2,1);
      (**(code **)(*param_1 + 0x10c))(param_1,param_2[1]);
      if ((*(byte *)(param_1 + 3) & 2) == 0) {
        System__PStrCpy_51F89FF7(local_10c,&DAT_55609974);
        System_TObject_ClassName_23EDC2EF(*param_1,local_20c);
        System__PStrCat_51F89FF7(local_10c,local_20c);
        System__PStrCat_51F89FF7(local_10c,&DAT_55609988);
        System__LStrFromString_EF219F52(&local_c,local_10c);
        SysUtils_IntToStr_0F6FDFF6(param_1[2],&local_210);
        System__LStrCat_51F89FF7(&local_c,local_210);
        SysUtils_Exception_Create_23EDC2EF(&HSEngine__EMapObjectResourceMissing_54E3FA20,1,local_c);
        System__RaiseExcept_51F89FF7();
      }
    }
  }
  puVar1 = puStack_220;
  *in_FS_OFFSET = uStack_228;
  puStack_220 = &LAB_55609965;
  puStack_224 = (undefined1 *)0x55609955;
  System__LStrClr_40929B27(&local_210,uStack_228,puVar1);
  puStack_224 = (undefined1 *)0x5560995d;
  System__LStrClr_40929B27(&local_c);
  return;
}

