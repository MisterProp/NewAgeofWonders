
/* WARNING: Unable to track spacebase fully for stack */

void FUN_55212eec(void)

{
  ushort *puVar1;
  uint uVar2;
  undefined1 *puVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  undefined1 *puVar8;
  undefined1 *puVar9;
  uint *puVar10;
  uint *puVar11;
  bool bVar12;
  int in_stack_00000004;
  int in_stack_00000008;
  uint *in_stack_0000000c;
  int in_stack_00000010;
  int in_stack_00000014;
  int in_stack_00000018;
  uint local_28;
  int local_24;
  int local_1c;
  undefined1 *local_8;
  
  puVar3 = (undefined1 *)(in_stack_00000008 + 0x1fU >> 5);
  iVar6 = in_stack_00000008 + in_stack_00000018;
  if (((-1 < iVar6 - *(int *)(in_stack_00000010 + 0x10)) &&
      (in_stack_00000018 < *(int *)(in_stack_00000010 + 0x18))) &&
     (*(int *)(in_stack_00000010 + 0x10) < *(int *)(in_stack_00000010 + 0x18))) {
    iVar7 = in_stack_00000004 + in_stack_00000014;
    if (((-1 < iVar7 - *(int *)(in_stack_00000010 + 0x14)) &&
        (in_stack_00000014 < *(int *)(in_stack_00000010 + 0x1c))) &&
       (*(int *)(in_stack_00000010 + 0x14) < *(int *)(in_stack_00000010 + 0x1c))) {
      iVar4 = iVar7 - *(int *)(in_stack_00000010 + 0x1c);
      if (iVar4 != 0 && *(int *)(in_stack_00000010 + 0x1c) <= iVar7) {
        in_stack_00000004 = in_stack_00000004 - iVar4;
      }
      iVar7 = *(int *)(in_stack_00000010 + 0x14) - in_stack_00000014;
      if (iVar7 != 0 && in_stack_00000014 <= *(int *)(in_stack_00000010 + 0x14)) {
        in_stack_00000014 = in_stack_00000014 + iVar7;
        in_stack_00000004 = in_stack_00000004 - iVar7;
        in_stack_0000000c = (uint *)((int)in_stack_0000000c + iVar7 * (int)puVar3 * 4);
      }
      if ((in_stack_00000018 - *(int *)(in_stack_00000010 + 0x10) < 0) ||
         (*(int *)(in_stack_00000010 + 0x18) < iVar6)) {
        local_24 = in_stack_00000008 * 2;
        uVar5 = *(int *)(in_stack_00000010 + 0x10) - in_stack_00000018;
        local_8 = puVar3;
        if ((int)uVar5 < 0) {
          local_28 = 0;
        }
        else {
          local_28 = uVar5 & 0x1f;
          uVar5 = uVar5 >> 5;
          if (uVar5 != 0) {
            local_8 = puVar3 + -uVar5;
            local_24 = local_24 + uVar5 * -0x40;
            in_stack_0000000c = in_stack_0000000c + uVar5;
            in_stack_00000018 = in_stack_00000018 + uVar5 * 0x20;
          }
        }
        uVar5 = iVar6 - *(int *)(in_stack_00000010 + 0x18);
        if (-1 < (int)uVar5) {
          local_24 = local_24 + uVar5 * -2;
          local_8 = local_8 + (-1 - (uVar5 >> 5));
        }
        iVar6 = *(int *)(in_stack_00000010 + 0xc);
        iVar7 = *(int *)(in_stack_00000010 + 0x20) + in_stack_00000018 * 2 +
                in_stack_00000014 * iVar6;
        do {
          local_1c = 0x40;
          iVar4 = 0;
          puVar8 = local_8;
          puVar10 = in_stack_0000000c;
          if (local_28 == 0) goto LAB_55213134;
          uVar5 = *in_stack_0000000c << (sbyte)local_28;
          iVar4 = local_28 * 2;
          puVar9 = local_8;
          puVar11 = in_stack_0000000c + 1;
          if (local_8 == (undefined1 *)0x0) {
LAB_552131d8:
            do {
              while( true ) {
                bVar12 = CARRY4(uVar5,uVar5);
                uVar5 = uVar5 * 2;
                if (bVar12) break;
                iVar4 = iVar4 + 2;
                if ((uVar5 == 0) || (local_24 <= iVar4)) goto LAB_55213211;
              }
              puVar1 = (ushort *)(iVar7 + iVar4);
              iVar4 = iVar4 + 2;
              *(ushort *)(iVar7 + iVar4) =
                   (*puVar1 >> 1 & 0x3def) + (short)((*puVar1 & 0x739c) >> 2);
            } while ((uVar5 != 0) && (iVar4 < local_24));
          }
          else {
LAB_55213136:
            while( true ) {
              uVar2 = uVar5 * 2;
              if (CARRY4(uVar5,uVar5)) break;
              if (uVar2 == 0) goto LAB_55213190;
              iVar4 = iVar4 + 2;
              uVar5 = uVar2;
            }
            if (uVar2 != 0) goto code_r0x5521313c;
            puVar10 = puVar11 + 1;
            puVar8 = puVar9 + -1;
            *(ushort *)(iVar7 + iVar4) =
                 (*(ushort *)(iVar7 + iVar4) >> 1 & 0x3def) +
                 (short)((*(ushort *)(iVar7 + iVar4) & 0x739c) >> 2);
            if (puVar9 != (undefined1 *)0x1) {
              iVar4 = local_1c;
              local_1c = local_1c + 0x40;
              goto LAB_55213134;
            }
LAB_552131ce:
            if (local_1c < local_24) {
              uVar5 = puVar11[1];
              iVar4 = local_1c;
              goto LAB_552131d8;
            }
          }
LAB_55213211:
          in_stack_00000004 = in_stack_00000004 + -1;
          if (in_stack_00000004 == 0) {
            return;
          }
          iVar7 = iVar7 + iVar6;
          in_stack_0000000c = in_stack_0000000c + (int)puVar3;
        } while( true );
      }
      iVar6 = *(int *)(in_stack_00000010 + 0xc);
      iVar7 = *(int *)(in_stack_00000010 + 0x20) + in_stack_00000018 * 2 + in_stack_00000014 * iVar6
      ;
LAB_55212fb6:
      local_1c = 0x40;
      iVar4 = 0;
      puVar8 = puVar3;
LAB_55212fc2:
      do {
        uVar5 = *in_stack_0000000c;
        do {
          while( true ) {
            while( true ) {
              uVar2 = uVar5 * 2;
              if (CARRY4(uVar5,uVar5)) break;
              if (uVar2 == 0) goto LAB_5521301e;
              iVar4 = iVar4 + 2;
              uVar5 = uVar2;
            }
            if (uVar2 == 0) {
              *(ushort *)(iVar7 + iVar4) =
                   (*(ushort *)(iVar7 + iVar4) >> 1 & 0x3def) +
                   (short)((*(ushort *)(iVar7 + iVar4) & 0x739c) >> 2);
              if (puVar8 == (undefined1 *)0x1) goto LAB_5521305c;
              iVar4 = local_1c;
              puVar8 = puVar8 + -1;
              in_stack_0000000c = in_stack_0000000c + 1;
              local_1c = local_1c + 0x40;
              goto LAB_55212fc2;
            }
            uVar5 = uVar5 * 4;
            if (!CARRY4(uVar2,uVar2)) break;
            puVar10 = (uint *)(iVar7 + iVar4);
            iVar4 = iVar4 + 4;
            *(uint *)(iVar7 + -4 + iVar4) =
                 (*puVar10 >> 1 & 0x3def3def) + ((*puVar10 & 0x739c739c) >> 2);
            if (uVar5 == 0) goto LAB_5521301e;
          }
          puVar1 = (ushort *)(iVar7 + iVar4);
          iVar4 = iVar4 + 4;
          *(ushort *)(iVar7 + iVar4) = (*puVar1 >> 1 & 0x3def) + (short)((*puVar1 & 0x739c) >> 2);
        } while (uVar5 != 0);
LAB_5521301e:
        if (puVar8 == (undefined1 *)0x1) goto LAB_5521305c;
        iVar4 = local_1c;
        puVar8 = puVar8 + -1;
        in_stack_0000000c = in_stack_0000000c + 1;
        local_1c = local_1c + 0x40;
      } while( true );
    }
  }
  return;
code_r0x5521313c:
  uVar5 = uVar5 * 4;
  if (CARRY4(uVar2,uVar2)) {
    *(uint *)(iVar7 + iVar4) =
         (*(uint *)(iVar7 + iVar4) >> 1 & 0x3def3def) +
         ((*(uint *)(iVar7 + iVar4) & 0x739c739c) >> 2);
  }
  else {
    *(ushort *)(iVar7 + iVar4 + 4) =
         (*(ushort *)(iVar7 + iVar4) >> 1 & 0x3def) +
         (short)((*(ushort *)(iVar7 + iVar4) & 0x739c) >> 2);
  }
  iVar4 = iVar4 + 4;
  if (uVar5 == 0) {
LAB_55213190:
    puVar10 = puVar11 + 1;
    puVar8 = puVar9 + -1;
    if (puVar9 == (undefined1 *)0x1) goto LAB_552131ce;
    iVar4 = local_1c;
    local_1c = local_1c + 0x40;
LAB_55213134:
    uVar5 = *puVar10;
    puVar9 = puVar8;
    puVar11 = puVar10;
  }
  goto LAB_55213136;
LAB_5521305c:
  in_stack_0000000c = in_stack_0000000c + 1;
  in_stack_00000004 = in_stack_00000004 + -1;
  if (in_stack_00000004 == 0) {
    return;
  }
  iVar7 = iVar7 + iVar6;
  goto LAB_55212fb6;
}

