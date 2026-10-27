

void HSEngine_TMapObject_SetResourceID_23EDC2EF(int param_1,undefined4 param_2)



{

  undefined4 extraout_EDX;

  

                    /* 0x9630  295  HSEngine.TMapObject.SetResourceID@23EDC2EF */

  if ((*(byte *)(param_1 + 0xc) & 2) != 0) {

    SysUtils_Exception_Create_23EDC2EF

              (SysUtils__Exception_9880D0BF_exref,1,"Cannot set ResourceID when resource is linked!"

              );

    param_1 = System__RaiseExcept_51F89FF7();

    param_2 = extraout_EDX;

  }

  *(undefined4 *)(param_1 + 8) = param_2;

  return;

}




