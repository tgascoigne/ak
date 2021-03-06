              .macro callback int
              .global idt_cb_\int
idt_cb_\int:
              push  $0              # dummy error code
              push  $\int
              jmp   idt_cb_common
              .endm

              .macro callback_err int
              .global idt_cb_\int
idt_cb_\int:
              push  $\int
              jmp   idt_cb_common
              .endm

              .extern idt_handler
idt_cb_common: pushal               # push all general registers

              mov   %ds, %ax
              push  %eax            # push data segment register
              mov   %esp, %edx      # store esp (idt_handler_args_t*)

              mov   $0x10, %ax      # load the kernel data segment
              mov   %ax, %ds
              mov   %ax, %es
              mov   %ax, %fs
              mov   %ax, %gs

              push  %edx
              call  idt_handler
              add   $4, %esp

              pop   %eax            # reload the callee's data segment
              mov   %ds, %ax
              mov   %es, %ax
              mov   %fs, %ax
              mov   %gs, %ax

              popal                 # pop all general registers
              add   $8, %esp        # drop the errno and interrupt number
              iret

              callback 0
              callback 1
              callback 2
              callback 3
              callback 4
              callback 5
              callback 6
              callback 7
              callback_err 8
              callback 9
              callback_err 10
              callback_err 11
              callback_err 12
              callback_err 13
              callback_err 14
              callback 15
              callback 16
              callback 17
              callback 18
              callback 19
              callback 20
              callback 21
              callback 22
              callback 23
              callback 24
              callback 25
              callback 26
              callback 27
              callback 28
              callback 29
              callback 30
              callback 31
              callback 32
              callback 33
              callback 34
              callback 35
              callback 36
              callback 37
              callback 38
              callback 39
              callback 40
              callback 41
              callback 42
              callback 43
              callback 44
              callback 45
              callback 46
              callback 47
              callback 48
              callback 49
              callback 50
              callback 51
              callback 52
              callback 53
              callback 54
              callback 55
              callback 56
              callback 57
              callback 58
              callback 59
              callback 60
              callback 61
              callback 62
              callback 63
              callback 64
              callback 65
              callback 66
              callback 67
              callback 68
              callback 69
              callback 70
              callback 71
              callback 72
              callback 73
              callback 74
              callback 75
              callback 76
              callback 77
              callback 78
              callback 79
              callback 80
              callback 81
              callback 82
              callback 83
              callback 84
              callback 85
              callback 86
              callback 87
              callback 88
              callback 89
              callback 90
              callback 91
              callback 92
              callback 93
              callback 94
              callback 95
              callback 96
              callback 97
              callback 98
              callback 99
              callback 100
              callback 101
              callback 102
              callback 103
              callback 104
              callback 105
              callback 106
              callback 107
              callback 108
              callback 109
              callback 110
              callback 111
              callback 112
              callback 113
              callback 114
              callback 115
              callback 116
              callback 117
              callback 118
              callback 119
              callback 120
              callback 121
              callback 122
              callback 123
              callback 124
              callback 125
              callback 126
              callback 127
              callback 128
              callback 129
              callback 130
              callback 131
              callback 132
              callback 133
              callback 134
              callback 135
              callback 136
              callback 137
              callback 138
              callback 139
              callback 140
              callback 141
              callback 142
              callback 143
              callback 144
              callback 145
              callback 146
              callback 147
              callback 148
              callback 149
              callback 150
              callback 151
              callback 152
              callback 153
              callback 154
              callback 155
              callback 156
              callback 157
              callback 158
              callback 159
              callback 160
              callback 161
              callback 162
              callback 163
              callback 164
              callback 165
              callback 166
              callback 167
              callback 168
              callback 169
              callback 170
              callback 171
              callback 172
              callback 173
              callback 174
              callback 175
              callback 176
              callback 177
              callback 178
              callback 179
              callback 180
              callback 181
              callback 182
              callback 183
              callback 184
              callback 185
              callback 186
              callback 187
              callback 188
              callback 189
              callback 190
              callback 191
              callback 192
              callback 193
              callback 194
              callback 195
              callback 196
              callback 197
              callback 198
              callback 199
              callback 200
              callback 201
              callback 202
              callback 203
              callback 204
              callback 205
              callback 206
              callback 207
              callback 208
              callback 209
              callback 210
              callback 211
              callback 212
              callback 213
              callback 214
              callback 215
              callback 216
              callback 217
              callback 218
              callback 219
              callback 220
              callback 221
              callback 222
              callback 223
              callback 224
              callback 225
              callback 226
              callback 227
              callback 228
              callback 229
              callback 230
              callback 231
              callback 232
              callback 233
              callback 234
              callback 235
              callback 236
              callback 237
              callback 238
              callback 239
              callback 240
              callback 241
              callback 242
              callback 243
              callback 244
              callback 245
              callback 246
              callback 247
              callback 248
              callback 249
              callback 250
              callback 251
              callback 252
              callback 253
              callback 254
              callback 255

              .section .rodata

              .global ISRRoots
ISRRoots:     #     A jump table indexing the idt callbacks
              .long idt_cb_0
              .long idt_cb_1
              .long idt_cb_2
              .long idt_cb_3
              .long idt_cb_4
              .long idt_cb_5
              .long idt_cb_6
              .long idt_cb_7
              .long idt_cb_8
              .long idt_cb_9
              .long idt_cb_10
              .long idt_cb_11
              .long idt_cb_12
              .long idt_cb_13
              .long idt_cb_14
              .long idt_cb_15
              .long idt_cb_16
              .long idt_cb_17
              .long idt_cb_18
              .long idt_cb_19
              .long idt_cb_20
              .long idt_cb_21
              .long idt_cb_22
              .long idt_cb_23
              .long idt_cb_24
              .long idt_cb_25
              .long idt_cb_26
              .long idt_cb_27
              .long idt_cb_28
              .long idt_cb_29
              .long idt_cb_30
              .long idt_cb_31
              .long idt_cb_32
              .long idt_cb_33
              .long idt_cb_34
              .long idt_cb_35
              .long idt_cb_36
              .long idt_cb_37
              .long idt_cb_38
              .long idt_cb_39
              .long idt_cb_40
              .long idt_cb_41
              .long idt_cb_42
              .long idt_cb_43
              .long idt_cb_44
              .long idt_cb_45
              .long idt_cb_46
              .long idt_cb_47
              .long idt_cb_48
              .long idt_cb_49
              .long idt_cb_50
              .long idt_cb_51
              .long idt_cb_52
              .long idt_cb_53
              .long idt_cb_54
              .long idt_cb_55
              .long idt_cb_56
              .long idt_cb_57
              .long idt_cb_58
              .long idt_cb_59
              .long idt_cb_60
              .long idt_cb_61
              .long idt_cb_62
              .long idt_cb_63
              .long idt_cb_64
              .long idt_cb_65
              .long idt_cb_66
              .long idt_cb_67
              .long idt_cb_68
              .long idt_cb_69
              .long idt_cb_70
              .long idt_cb_71
              .long idt_cb_72
              .long idt_cb_73
              .long idt_cb_74
              .long idt_cb_75
              .long idt_cb_76
              .long idt_cb_77
              .long idt_cb_78
              .long idt_cb_79
              .long idt_cb_80
              .long idt_cb_81
              .long idt_cb_82
              .long idt_cb_83
              .long idt_cb_84
              .long idt_cb_85
              .long idt_cb_86
              .long idt_cb_87
              .long idt_cb_88
              .long idt_cb_89
              .long idt_cb_90
              .long idt_cb_91
              .long idt_cb_92
              .long idt_cb_93
              .long idt_cb_94
              .long idt_cb_95
              .long idt_cb_96
              .long idt_cb_97
              .long idt_cb_98
              .long idt_cb_99
              .long idt_cb_100
              .long idt_cb_101
              .long idt_cb_102
              .long idt_cb_103
              .long idt_cb_104
              .long idt_cb_105
              .long idt_cb_106
              .long idt_cb_107
              .long idt_cb_108
              .long idt_cb_109
              .long idt_cb_110
              .long idt_cb_111
              .long idt_cb_112
              .long idt_cb_113
              .long idt_cb_114
              .long idt_cb_115
              .long idt_cb_116
              .long idt_cb_117
              .long idt_cb_118
              .long idt_cb_119
              .long idt_cb_120
              .long idt_cb_121
              .long idt_cb_122
              .long idt_cb_123
              .long idt_cb_124
              .long idt_cb_125
              .long idt_cb_126
              .long idt_cb_127
              .long idt_cb_128
              .long idt_cb_129
              .long idt_cb_130
              .long idt_cb_131
              .long idt_cb_132
              .long idt_cb_133
              .long idt_cb_134
              .long idt_cb_135
              .long idt_cb_136
              .long idt_cb_137
              .long idt_cb_138
              .long idt_cb_139
              .long idt_cb_140
              .long idt_cb_141
              .long idt_cb_142
              .long idt_cb_143
              .long idt_cb_144
              .long idt_cb_145
              .long idt_cb_146
              .long idt_cb_147
              .long idt_cb_148
              .long idt_cb_149
              .long idt_cb_150
              .long idt_cb_151
              .long idt_cb_152
              .long idt_cb_153
              .long idt_cb_154
              .long idt_cb_155
              .long idt_cb_156
              .long idt_cb_157
              .long idt_cb_158
              .long idt_cb_159
              .long idt_cb_160
              .long idt_cb_161
              .long idt_cb_162
              .long idt_cb_163
              .long idt_cb_164
              .long idt_cb_165
              .long idt_cb_166
              .long idt_cb_167
              .long idt_cb_168
              .long idt_cb_169
              .long idt_cb_170
              .long idt_cb_171
              .long idt_cb_172
              .long idt_cb_173
              .long idt_cb_174
              .long idt_cb_175
              .long idt_cb_176
              .long idt_cb_177
              .long idt_cb_178
              .long idt_cb_179
              .long idt_cb_180
              .long idt_cb_181
              .long idt_cb_182
              .long idt_cb_183
              .long idt_cb_184
              .long idt_cb_185
              .long idt_cb_186
              .long idt_cb_187
              .long idt_cb_188
              .long idt_cb_189
              .long idt_cb_190
              .long idt_cb_191
              .long idt_cb_192
              .long idt_cb_193
              .long idt_cb_194
              .long idt_cb_195
              .long idt_cb_196
              .long idt_cb_197
              .long idt_cb_198
              .long idt_cb_199
              .long idt_cb_200
              .long idt_cb_201
              .long idt_cb_202
              .long idt_cb_203
              .long idt_cb_204
              .long idt_cb_205
              .long idt_cb_206
              .long idt_cb_207
              .long idt_cb_208
              .long idt_cb_209
              .long idt_cb_210
              .long idt_cb_211
              .long idt_cb_212
              .long idt_cb_213
              .long idt_cb_214
              .long idt_cb_215
              .long idt_cb_216
              .long idt_cb_217
              .long idt_cb_218
              .long idt_cb_219
              .long idt_cb_220
              .long idt_cb_221
              .long idt_cb_222
              .long idt_cb_223
              .long idt_cb_224
              .long idt_cb_225
              .long idt_cb_226
              .long idt_cb_227
              .long idt_cb_228
              .long idt_cb_229
              .long idt_cb_230
              .long idt_cb_231
              .long idt_cb_232
              .long idt_cb_233
              .long idt_cb_234
              .long idt_cb_235
              .long idt_cb_236
              .long idt_cb_237
              .long idt_cb_238
              .long idt_cb_239
              .long idt_cb_240
              .long idt_cb_241
              .long idt_cb_242
              .long idt_cb_243
              .long idt_cb_244
              .long idt_cb_245
              .long idt_cb_246
              .long idt_cb_247
              .long idt_cb_248
              .long idt_cb_249
              .long idt_cb_250
              .long idt_cb_251
              .long idt_cb_252
              .long idt_cb_253
              .long idt_cb_254
              .long idt_cb_255
