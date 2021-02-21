# ChinoVM汇编语言
ChinoVM是为ChinoC编译器开发的一种将高级语言转换为汇编语言的一种中间汇编语言
## 汇编指令

### mov指令
```asm
    #格式: mov dst,src
    #副作用:将src的值赋给dst
    mov a,10 #将10送入变量a
    mov a,b  #将变量b的值送入变量a
    mov a,&b #取变量b的地址
    mov a,*b #取存放在变量b中的地址的值
    mov *a,b #把b的值赋给变量a指向的地址的值
```

### sub指令
```asm
    #格式:sub dst,src1,src2
    #副作用:将src1-src2的值赋给dst
    sub a,b,c #a = b - c
```
### add指令
```asm
    #格式：add dst,src1,src2
    #副作用:将src1+src2的值赋给dst
    add a,b,c # a = b + c
```
### mul指令
```asm
    #格式：mul dst,src1,src2
    #副作用:将src1*src2的值赋给dst
    mul a,b,c #a = b * c
```

### div指令
```asm
    #格式：div dst,src1,src2
    #副作用:将src1/src2的值赋给dst
    div a,b,c #a = b / c
```

### and指令
```asm
    #格式：and dst,src1,src2
    #副作用:将src1 & src2的值赋给dst
    and a,b,c #a = b & c
```

### or指令
```asm
    #格式：and dst,src1,src2
    #副作用:将src1 | src2的值赋给dst
    or a,b,c #a = b | c
```

### xor指令
```asm
    #格式：xor dst,src
    #副作用:将~src的值赋给dst
    xor a,b,c #a = b ^ c
```

### not指令
```asm
    #格式：not dst,src
    #副作用:将~src的值赋给dst
    not a,b #a = ~b
```

### shl指令
```asm
    #格式：shl dst,src1,src2
    #副作用:将src1 << src2的值赋给dst
    div a,b,c #a = b << c
```

### shr指令
```asm
    #格式：shr dst,src1,src2
    #副作用:将src1 >> src2的值赋给dst
    div a,b,c #a = b >> c
```

### jmp指令
```asm
    #格式：jmp dst
    #副作用：跳转到地址为dst的地方，dst可以为标号(地址)，也可以为变量
    jmp a #跳转到标号a的地方
    jmp b #跳转到变量b存放地址的地方
```

### jne指令
```asm
    #格式：jne dst,src1,src2
    #副作用：如果src1 != src2则跳转到dst，dst可以为标号(地址)，也可以为变量
```

### je指令
```asm
    #格式：je dst,src1,src2
    #副作用：如果src1 == src2则跳转到dst，dst可以为标号(地址)，也可以为变量
```

### jg指令
```asm
    #格式：jg dst,src1,src2
    #副作用：如果src1 > src2则跳转到dst，dst可以为标号(地址)，也可以为变量
```

### jng指令
```asm
    #格式：jng dst,src1,src2
    #副作用：如果src1 >= src2则跳转到dst，dst可以为标号(地址)，也可以为变量
```

### jl指令
```asm
    #格式：jl dst,src1,src2
    #副作用：如果src1 < src2则跳转到dst，dst可以为标号(地址)，也可以为变量
```

### jnl指令
```asm
    #格式：jnl dst,src1,src2
    #副作用：如果src1 <= src2则跳转到dst，dst可以为标号(地址)，也可以为变量
```

### call指令
```asm
    #格式：call dst
    #副作用：将当前地址压栈，并且跳转到dst
```

### ret指令
```asm
    #格式：ret
    #副作用：从栈中弹出一个地址，并跳转到弹出的地址
```
### push指令
```asm
    #格式：push value
    #副作用：将一个值推入栈中
    push a #将变量a的值推入栈中
    push 10 #将立即数10推入栈中
```
### pop指令
```asm
    #格式：pop dst
    #副作用：将栈中的值弹出到dst中
    pop a #从栈中弹出一个值，并赋给a
```

## 其他指令
```asm
    .var bytes number name #声明一个变量
    #bytes指向变量占用的内存
    #number大于1则为数组
```