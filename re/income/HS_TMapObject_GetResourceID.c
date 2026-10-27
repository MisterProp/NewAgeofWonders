

undefined4 HSEngine_TMapObject_GetResourceID_23EDC2EF(int param_1)



{

                    /* 0x961c  296  HSEngine.TMapObject.GetResourceID@23EDC2EF */

  if ((*(byte *)(param_1 + 0xc) & 2) != 0) {

    return *(undefined4 *)(*(int *)(param_1 + 8) + 0xc);

  }

  return *(undefined4 *)(param_1 + 8);

}




