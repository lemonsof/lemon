/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/*******************************************************************
 *                                                                 *
 *  -------------------------------------------------------------  *
 *  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  *
 *  -------------------------------------------------------------  *
 *  |  0  |  4  |  8  |  12 |  16 |  20 |  24 |  28 |  32 |  36 |  *
 *  -------------------------------------------------------------  *
 *  | R13 | R14 | R15 | R16 | R17 | R18 | R19 | R20 | R21 | R22 |  *
 *  -------------------------------------------------------------  *
 *  -------------------------------------------------------------  *
 *  |  10 |  11 |  12 |  13 |  14 |  15 |  16 |  17 |  18 |  19 |  *
 *  -------------------------------------------------------------  *
 *  |  40 |  44 |  48 |  52 |  56 |  60 |  64 |  68 |  72 |  76 |  *
 *  -------------------------------------------------------------  *
 *  | R23 | R24 | R25 | R26 | R27 | R28 | R29 | R30 | R31 |  SP |  *
 *  -------------------------------------------------------------  *
 *  -------------------------------------------------------------  *
 *  |  20 |  21 |  22 |                                         |  *
 *  -------------------------------------------------------------  *
 *  |  80 |  84 |  88 |                                         |  *
 *  -------------------------------------------------------------  *
 *  |  CR |  LR |  PC |                                         |  *
 *  -------------------------------------------------------------  *
 *  -------------------------------------------------------------  *
 *  |  23 |  24 |  25 |                                         |  *
 *  -------------------------------------------------------------  *
 *  |  92 |  96 | 100 |                                         |  *
 *  -------------------------------------------------------------  *
 *  |  sp | size|<pad>|                                         |  *
 *  -------------------------------------------------------------  *
 *  -------------------------------------------------------------  *
 *  |  26 |  27 |  28 |  29 |  30 |  31 |  32 |  33 |  34 |  35 |  *
 *  -------------------------------------------------------------  *
 *  | 104 | 108 | 112 | 116 | 120 | 124 | 128 | 132 | 136 | 140 |  *
 *  -------------------------------------------------------------  *
 *  |    F14    |    F15    |    F16    |    F17    |    F18    |  *
 *  -------------------------------------------------------------  *
 *  -------------------------------------------------------------  *
 *  |  36 |  37 |  38 |  39 |  40 |  41 |  42 |  43 |  44 |  45 |  *
 *  -------------------------------------------------------------  *
 *  | 144 | 148 | 152 | 156 | 160 | 164 | 168 | 172 | 176 | 180 |  *
 *  -------------------------------------------------------------  *
 *  |    F19    |    F20    |    F21    |    F22    |    F23    |  *
 *  -------------------------------------------------------------  *
 *  -------------------------------------------------------------  *
 *  |  46 |  47 |  48 |  49 |  50 |  51 |  52 |  53 |  54 |  55 |  *
 *  -------------------------------------------------------------  *
 *  | 184 | 188 | 192 | 196 | 200 | 204 | 208 | 212 | 216 | 220 |  *
 *  -------------------------------------------------------------  *
 *  |    F24    |    F25    |    F26    |    F27    |    F28    |  *
 *  -------------------------------------------------------------  *
 *  -------------------------------------------------------------  *
 *  |  56 |  57 |  58 |  59 |  60 |  61 |  62 |  63 |           |  *
 *  -------------------------------------------------------------  *
 *  | 224 | 228 | 232 | 236 | 240 | 244 | 248 | 252 |           |  *
 *  -------------------------------------------------------------  *
 *  |    F29    |    F30    |    F31    |   fpscr   |           |  *
 *  -------------------------------------------------------------  *
 *                                                                 *
 * *****************************************************************/

.text
.globl sof_context_jump
.align 2
.type sof_context_jump,@function
sof_context_jump:
    stw     %r13,  0(%r3)       # save R13
    stw     %r14,  4(%r3)       # save R14
    stw     %r15,  8(%r3)       # save R15
    stw     %r16,  12(%r3)      # save R16
    stw     %r17,  16(%r3)      # save R17
    stw     %r18,  20(%r3)      # save R18
    stw     %r19,  24(%r3)      # save R19
    stw     %r20,  28(%r3)      # save R20
    stw     %r21,  32(%r3)      # save R21
    stw     %r22,  36(%r3)      # save R22
    stw     %r23,  40(%r3)      # save R23
    stw     %r24,  44(%r3)      # save R24
    stw     %r25,  48(%r3)      # save R25
    stw     %r26,  52(%r3)      # save R26
    stw     %r27,  56(%r3)      # save R27
    stw     %r28,  60(%r3)      # save R28
    stw     %r29,  64(%r3)      # save R29
    stw     %r30,  68(%r3)      # save R30
    stw     %r31,  72(%r3)      # save R31
    stw     %r1,   76(%r3)      # save SP

    mfcr    %r0                 # load CR
    stw     %r0,   80(%r3)      # save CR
    mflr    %r0                 # load LR
    stw     %r0,   84(%r3)      # save LR
    stw     %r0,   88(%r3)      # save LR as PC

    cmpwi   cr7,   %r6,  0      # test if fpu env should be preserved
    beq     cr7,   1f

    stfd    %f14,  104(%r3)     # save F14
    stfd    %f15,  112(%r3)     # save F15
    stfd    %f16,  120(%r3)     # save F16
    stfd    %f17,  128(%r3)     # save F17
    stfd    %f18,  136(%r3)     # save F18
    stfd    %f19,  144(%r3)     # save F19
    stfd    %f20,  152(%r3)     # save F20
    stfd    %f21,  160(%r3)     # save F21
    stfd    %f22,  168(%r3)     # save F22
    stfd    %f23,  176(%r3)     # save F23
    stfd    %f24,  184(%r3)     # save F24
    stfd    %f25,  192(%r3)     # save F25
    stfd    %f26,  200(%r3)     # save F26
    stfd    %f27,  208(%r3)     # save F27
    stfd    %f28,  216(%r3)     # save F28
    stfd    %f29,  224(%r3)     # save F29
    stfd    %f30,  232(%r3)     # save F30
    stfd    %f31,  240(%r3)     # save F31
    mffs    %f0                 # load FPSCR
    stfd    %f0,   248(%r3)     # save FPSCR

    lfd     %f14,  104(%r4)     # restore F14
    lfd     %f15,  112(%r4)     # restore F15
    lfd     %f16,  120(%r4)     # restore F16
    lfd     %f17,  128(%r4)     # restore F17
    lfd     %f18,  136(%r4)     # restore F18
    lfd     %f19,  144(%r4)     # restore F19
    lfd     %f20,  152(%r4)     # restore F20
    lfd     %f21,  160(%r4)     # restore F21
    lfd     %f22,  168(%r4)     # restore F22
    lfd     %f23,  176(%r4)     # restore F23
    lfd     %f24,  184(%r4)     # restore F24
    lfd     %f25,  192(%r4)     # restore F25
    lfd     %f26,  200(%r4)     # restore F26
    lfd     %f27,  208(%r4)     # restore F27
    lfd     %f28,  216(%r4)     # restore F28
    lfd     %f29,  224(%r4)     # restore F29
    lfd     %f30,  232(%r4)     # restore F30
    lfd     %f31,  240(%r4)     # restore F31
    lfd     %f0,   248(%r4)     # load FPSCR
    mtfsf   0xff,  %f0          # restore FPSCR
1:

    lwz     %r13,  0(%r4)       # restore R13
    lwz     %r14,  4(%r4)       # restore R14
    lwz     %r15,  8(%r4)       # restore R15
    lwz     %r16,  12(%r4)      # restore R16
    lwz     %r17,  16(%r4)      # restore R17
    lwz     %r18,  20(%r4)      # restore R18
    lwz     %r19,  24(%r4)      # restore R19
    lwz     %r20,  28(%r4)      # restore R20
    lwz     %r21,  32(%r4)      # restore R21
    lwz     %r22,  36(%r4)      # restore R22
    lwz     %r23,  40(%r4)      # restore R23
    lwz     %r24,  44(%r4)      # restore R24
    lwz     %r25,  48(%r4)      # restore R25
    lwz     %r26,  52(%r4)      # restore R26
    lwz     %r27,  56(%r4)      # restore R27
    lwz     %r28,  60(%r4)      # restore R28
    lwz     %r29,  64(%r4)      # restore R29
    lwz     %r30,  68(%r4)      # restore R30
    lwz     %r31,  72(%r4)      # restore R31
    lwz     %r1,   76(%r4)      # restore SP

    lwz     %r0,   80(%r4)      # load CR
    mtcr    %r0                 # restore CR
    lwz     %r0,   84(%r4)      # load LR
    mtlr    %r0                 # restore LR

    mr      %r3,   %r5          # use third arg as return value after jump
                                # and as first arg in context function

    lwz     %r0,   88(%r4)      # load PC
    mtctr   %r0                 # restore CTR

    bctr                        # jump to context
.size sof_context_jump, .-sof_context_jump
