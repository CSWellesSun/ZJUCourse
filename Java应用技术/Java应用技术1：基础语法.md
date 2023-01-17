## Java应用技术1：基础语法

###### RandomStar

##### 成绩组成

- 平时分70%+期末考试30%
  - 包含出勤，PTA作业，编程小作业，大作业，期中考试
- `ftp://java:java123456@10.15.62.53`



#### Chapter 1：Introduction

##### 1.1 Java语言的特性

- 简洁
  - 没有C/C++中的指针，没有多重继承和运算符重载的语法特性
- 面向对象
  - Java设计的核心是如何复用代码，并且支持继承封装多态
- Interpreted
  - 代码会编译成字节码(bytecode)在Java虚拟机上运行
  - Java是跨平台的，可以在所有有JVM的计算机上运行，JMV可以设置不同的安全等级
- 鲁棒性
  - 强类型机制，异常处理，垃圾内存自动搜集等机制

##### 1.2 JDK的版本

- Java Standard Edition (J2SE)
  - 可以用于开发客户端应用和app
- Java Enterprise Edition (J2EE)
  - 可以用于开发服务端应用，比如Java Servlets
- Java Micro Edition (J2ME)
  - 开发手机应用
- 本课程中的内容主要是**J2SE** 

##### 1.3 Java程序的组成

- 一个简单的Java程序

```java
public class HelloWorld{
    public static void main(String[] args) {
        System.out.println("Hello World!");
    }
}
```

- Java代码编译的过程
  - Java代码文件的后缀名是`.java`，被Java编译器编译之后编译成`.class`文件，之后在JVM中运行
- 每个Java程序至少需要一个class，每个class都有唯一的class name
  - block是java程序的组件，比如一个类就是一个class block
- Java程序中的错误
  - 语法错误Syntax Error：会在编译期间被检查出来，因此有语法错误的时候无法通过编译
  - 运行时错误 Run time Error：
  - 程序逻辑上本身的错误

##### 1.4 JVM

速度慢在：

- 解释执行
- Code Check，运行字节码之前会check防止出现错误指令
- Java编译之后的字节码以一个字节作为一条指令，采用堆栈而不是寄存器来执行，导致内存中运行较慢一些

速度快在：

- JIT：just-in-time，将字节码翻译成机器码，局部性原则
- 多线程
- 字节码很少

##### ⭐作业里一道莫名其妙的题目

- 对于下面的这样一个Java程序，它**可以通过编译**但是运行的时候会因为**没有入口main函数**而出现错误
  - 这是因为Java的编译器会把类编译成class文件，下面这个类是一个合法的类，没有语法错误，因此可以通过编译
  - 但是在JVM中执行的时候会因为没有入口main函数而出现错误

```Java
public class main { // 与类名无关，没有public也可以
    public void main() {
        System.out.println("Hello World!");
    }
    /* 只有下面这种可以，其他都找不到main
    public static void main(String[] args) {
    	...
    }
    */
}
```



#### 2. Java的基本语法

##### 2.1 Java的变量命名

- 变量：包含大小写字母，数字，下划线和**$符号**

  - Java中的变量不能用数字开头，也不能是保留字

- 变量的定义和声明

  - Java中**不区分变量的声明和定义**，这是和C/C++的最大不同
    - C/C++中的编译器**不会为基本类型赋予初始默认值**
    - Java的声明就会开空间
  - Java对于方法的局部变量，Java以**编译时错误**来保证**变量在使用前都能得到恰当的初始化 ** 
    - 但是Java对于方法内的单个变量不会赋予初始值，对于数组会赋予默认的值
    - 成员变量有初始值
  
- 对于下面这样一段代码
  
  - 输出的结果是0，因为**变量进行了初始化但是没有赋予具体值**
  - Java中int类型的默认值就是0
  
  ```Java
  public class JavaPractice {
      static int array[] = new int[5];
      public static void main(String a[]) {
          System.out.println(array[0]);
      }
  }
  ```
  
- **常量**定义`final datatype CONSTANTNAME = VALUE;`
    - final的主要用法有如下三种
      - 修饰变量，表示变量的值**不能改变**，但是可以进行任何合法形式的初始化，相当于C/C++中的const类型
        - `final int i = 1` 编译时初始化
        - `final int i2 = (int)(Math.Random() * 10)` 运行时
        - `final int i3` 构造函数再初始化
      - 修饰类对象，表示这个变量不能再赋值成其他的对象，比如一个对象被new了之后，就不能再把它new成一个新的对象。类似C++的const *p
    - 修饰方法method，表示一个Java函数不可更改，**不能被重载** 
    - 修饰类，表示这个类**不能被继承**，类中的所有方法也就变成了final类型的
  
- Java中的变量类型

  - 数值型变量：Java中整数类型的范围和**运行的机器无关**，这一点和C/C++不同，没有无符号数（原因特殊设计了**逻辑移位**）
    - byte 8bit 有符号类型
    - short 16bit 有符号类型
    - int 32bit有符号类型
    - long 64bit有符号类型
    - float和double分别是32bit和64bit的IEEE754标准

  - C的unsigned是为了表示逻辑移位，java使用`>>>`

    ![image-20230108210705283](Java应用技术1：基础语法.assets/image-20230108210705283.png)

    输出 -2 -2

    因为2147483647*2默认就是int

    2147483647+1 = -2147483648

  - 大的字面量赋给一个小的类型会产生编译错误。

  - 整数字面量默认为$-2^{31}$到$2^{31}-1$，如果要用64bit需要加上l或者L

  - 浮点数字面量默认为double，可以后面加f/F和d/D

  - 特殊的浮点数：`Double.POSITIVE_INFINITY/Double.NEGATIVE_INFINITY/Double.NaN`

  - 数据类型转换的时候：如果不产生丢失，可以直接等于；否则就要显式声明，具体如下：

    ![image-20221014154606126](Java应用技术1：基础语法.assets/image-20221014154606126.png)

  - 四舍五入：`Math.round`

  - **字符类型char** 

    - 编码方式是Unicode
    - Java中的一个char类型变量占16bits，也就是2个字节
    - 当注释中出现`\u`还是会变成Unicode，注意如果注释中\u后面不是4个十六进制数就出现语法错误

- 数值类型的读入

  - 首先需要定义Scanner读入器`Scanner input = new Scanner(System.in)`

  - 之后可以用input的`nextInt`方法读取下一个整数，`nextLine`读入一整行，`next`读入一个单词（单词的标志是空格、tab或者换行）

  - ```java
    import java.util.Scanner;
    
    public class A {
        public static void main(String[] args) {
    		Scanner in = new Scanner(System.in);
            int i = in.nextInt();
            System.out.println("Hello World!" + i + "\n"); // 自动调用toString
            in.close();
        }
    }
    ```

  - ```java
    public static void main(String[] argv){
        String s = "16 abc 32 de 64";
        Scanner sc = new Scanner(s);
        while ( sc.hasNext() ) {
            if ( sc.hasNextInt() ) {
                System.out.printf("%02X",sc.nextInt());
            } else {
                System.out.print(sc.next());
            }
        }
    }
    10abc20de40
    ```

- Java中的数值运算：支持加减乘除取余等各种操作

  - 除法中如整数除法的结果是整数，浮点数除法的结果浮点数

  - 浮点数中double比float更加精确，double精确到16位，float精确到8位小数
  - 几个特殊的浮点数值
    - `Double.POSITIVE_INFINITY` 正无穷大
    - `Double.NEGATIVE_INFINITY` 负无穷大
    - `Double.NaN` 不是数字
    - 可以用Double.isNaN来判断是否为数字

- 类型转换：byte，short，int，float，double，long，char之间可以进行类型转换

  - 其中整形向浮点型的转换可能会造成精度的损失
  - 浮点数向整形转换的时候会丢弃小数部分
  - Java不支持C++中的自动强制类型转换，有需要的类型转换必须显式地声明：加宽转换一般是隐式的，缩窄必须是显式的（long->int）

##### 2.2 选择语句

- 布尔类型变量(boolean) 值只有true和false，Java中的大小关系符和C/C++一致

###### 2.2.1 if语句和switch语句

- 和C/C++基本一致，没啥好学的

- if语句有单个if，if-else，多层嵌套if等写法，**else和最近的if匹配** 

- switch语句也跟C/C++基本一致，有break有default

  - 只能byte/short/char/int，java1.7之后增加了string。支持enum
  - long和布尔类型不能用在switch的选择里，下面的代码是错误的
  
  ```Java
  boolean x;
  switch(x) {
  	// 
  }
  ```

###### 2.2.2 逻辑运算符

- Java中有如下逻辑运算符

  - ！逻辑否，&&逻辑且，|| 逻辑或，**^逻辑异或** 
  - &&和||的运算按照短路的方式来求值，如果第一个操作数已经可以确定表达式的值，后面的就不需要进行运算了
  - &和|也可以进行逻辑运算，区别是这两个**不用短路**的方式来求值
  - ![image (2)](Java应用技术1：基础语法.assets/image (2).png)


##### 2.3 数学函数

- 常见的两个常数：PI和E表示圆周率和自然对数的底数
- 常见的数学类方法
  - 三角函数
  - 幂
  - 高斯函数和舍入方法：`double rint(double x)`取离x最近的整数，如果一样近取偶数
  - 最大，最小，绝对值和随机：`random()`返回值`[0.0,1.0)`
  - Exp/Log/log10/Pow/Sqrt/Ceil/Floor/Rint/Round

##### 2.4 字符和字符串类型

- Java中的字符类型有两种表示方式，ASCII编码和Unicode编码模式
  - Unicode编码模式的形式是前缀`\u`+四位十六进制数，可以表示从0000到FFFF一共65536个字符
  - 常见的ASCII码
    - '0' - '9'在ASCII码中式48到57
    - 'A' - 'Z' 式65-90， 'a'-'z'是97-122
  
- `String s;`，这里是一个指针，并且没有指向任何东西

- ![image-20220923220927064](Java应用技术1：基础语法.assets/image-20220923220927064.png)

  `object`都有`toString`函数，而对于primitive type有"type wrapper" class，比如int有Integer，然后再调用`Integer.toString()`。

  最下面两行是根据从左向右计算的顺序

- 字符串内置函数

  ![image (3)](Java应用技术1：基础语法.assets/image (3).png)
  
  ![image (4)](Java应用技术1：基础语法.assets/image (4).png)
  
  ![image (5)](Java应用技术1：基础语法.assets/image (5).png)
  
- 字符串相等，使用`String.equals(String)`，因为是指针（另外还有`equalsIgnoreCase`）

  ```java
  String s1 = "hello";
  String s2 = "hello";
  s1 == s2; // true
  
  String s1 = "hello";
  String s2 = "hel" + "lo"; // 编译器计算完了
  s1 == s2; // true
  
  String s1 = "hello";
  String s2 = "hel";
  s2 += "lo";
  s1 == s2; // false
  
  String s1 = new String("hello");
  String s2 = new String("hello");
  s1 == s2; // false
  ```

  ```java
  public static void f(String s1) {
      String s2 = "hello";
      s1 == s2; // true
  }
  
  public static void main(String[] args) {
      String s1 = "hello";
      f(s1);
  }
  ```

- 字符串内置方法

  - `equals/equalsIgnoreCase`

  - `compareTo/compareToIgnoreCase`
  - `charAt`
  - `substring(int beginIndex, int endIndex)`，其中`endIndex`不包括在里面
  - `indexOf(int character)`
  - `lastIndexOf(int character)`
  - `indexOf(int character, int from)`
  - `lastIndexOf(int character, int from)`
  - `getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin)`
  - ![image-20220923222655381](Java应用技术1：基础语法.assets/image-20220923222655381.png)
  - `String.format`

- 字符串类型 String

  - 读取字符串的方式：
    - 使用next() 从有效字符开始扫描，遇到第一个分隔符或者结束符的时候结束，将结果作为字符串返回
    - nextLine扫描当前行所有的字符串作为结果返回
  - 获取字串，使用`substring`方法，必须要有的参数是起始位置beginIndex，结束位置endIndex可以缺省，默认值是字符串末尾，endIndex不包括

  - 字符串类型是不可变immutable的，不能对String中的内容做出改变，同时如果在函数中对String进行赋值操作也不能改变主函数里的String，比如下面这样一段代码，最后的输出还是A,B。**因为`String`是指针！**

  ```java
  public class Main {
      public static void main(String args[]) {
          String a = new String("A");
          String b = new String("B");
          swap(a, b);
          System.out.println(a + "." + b);
      }
      static void swap(String x, String y) {
          y = x;
      }
  }
  ```

  - String的concat和substring等方法都不是在原来的字符串上操作的，而是生成了一个新的字符串

  - ```java
    String s1 = " hello ";
    s1.trim();
    System.out.println("##" + s1 + "##");
    // ## hello ##
    // s1并没有被trim掉，因为是immutable
    ```

- 转成String的方法：

  ```java
  ""+12
  Interger.toSrting(12)
  String.valueOf(12)
  ```
  
- 将字符串转成数字

  - `Interger Integer.valueOf(String);`返回值是类的对象
  - `int Integer.parseInt`
  
- 格式化输出

  - `System.out.printf(format, items);` 具体的用法和C语言的printf类似

  - 占位符的具体格式包括

    - `%[index$][标识]*[最小宽度][.精度]转换符` 

    - index表示从第几个位置开始计算来进行格式化，起始为1

    - 最小宽度是格式化之后最小的长度，当输出结果小于最小宽度的时候用标识符填补空格，没有标识符的时候用空格填充

      - 字符串可用标识：- 表示左对齐，默认的是右对齐
      - 整数和浮点数可用标识：

      ![image (6)](Java应用技术1：基础语法.assets/image (6).png)

      ![image (7)](Java应用技术1：基础语法.assets/image (7).png)

      - 对日期进行格式化

        ![image-20221014171610508](Java应用技术1：基础语法.assets/image-20221014171610508.png)
    
    - 精度用于设置浮点数保留几位小数
    
    - 转换符用于指定转化的格式，有f，d等等

##### 2.5 循环

- 和C/C++基本一致，有for循环，while循环和do-while循环
  - 要注意区别while循环和do-while循环
  - `for(;;){}`和`while(true){}`等价
  - Java中也有`break`和`continue`来结束或者跳出循环
  - break/continue后面可以加label来选择跳出哪个block，注意不能跳进
    - label的设置可以给整个块加个名字，也可以给`for`等加个名字与冒号

- 在Java8中，可以利用`range()`方法来创建流

  ```java
  IntStream st = IntStream.range(12, 15);
  st.foreach(System.out.println);
  ```

  

##### 2.6 方法

- 将程序中的一部分过程抽象成一个方法，起到代码复用的作用，一个方法的定义包含如下如内容
  - modifier 修饰符，包括方法的public/private/protected和是否是static，final等属性
  
    - Java修饰符不只是修饰变量，也可以修饰类
    - friendly：默认是friendly，同一个package可以访问
    - Public：所有人都可以访问
    - Private：内部类
    - Protected：
      - C++ 子类可以访问
      - Java 同一个package以及子类可以访问
  
  - return value type 返回值类型
  
  - method name 方法名
  
  - formal parameters 形式参数
    - 方法名+形式参数称为方法的签名
    - 与之相对应的是actual parameter or argument实际参数，也就是调用方法的时候使用的参数
    - 实参的内容不会因为方法中的操作被改变
  
  - body 方法的具体内容
  
    ```java
    modifier 返回类型 方法名称([参数类型 变量, ......]) {
            方法体代码;
            [return [返回值];]
    }
    ```
  
- 方法的重载：相同的方法名，不同的参数表构成重载关系
  - 只有返回值类型不相同的时候不构成重载，这种写法会引起编译错误，不能使用
  - 重载可以有返回值类型的区别，但是一定会有**参数表的区别** 
  
- 局部变量：定义在方法内部的变量叫做局部变量
  - 局部变量的可调用范围是从定义开始到不再包含这个变量的方法内部区域为止(比如for循环中定义的变量只能在for循环中调用) 
  
  - Java对于方法的局部变量，Java以**编译时错误**来保证**变量在使用前都能得到恰当的初始化**  
  
    - 但是Java对于方法内的单个变量**不会赋予初始值，对于数组会赋予默认的值**
  
    ```java
    import java.util.Scanner;
    public class test {
        public static void main(String[] args) {
            f();
        }
        public static void f()
        {
            double[] list = new double [5];
            int i;
            System.out.println(list[0]);//不会报错，默认赋值
            System.out.println(i);//会直接报错
        }
    }
    // 注意：只有用到i的时候才会报错
    ```
  
    

##### 2.7 数组

###### 2.7.1 一维数组

- Java中数组的定义方式`datatype[ ] array`，中括号也可以放在后面，然后需要用`new datatype[size]`来声明数组的大小

  ```java
  int[] arr = new int [5];
  int[] arr1={1,2,3,4,5};//简化格式
  int arr2[] = {1,2,3,4,5};
  int[] arr3 = new int []{1,2,3,4,5};//完整格式
  
  int arr4[2] = {1,2};//错误
  ```
  
  - 可以用length来获得数组的长度，但要注意这个和String类型的`length()` **方法**不一样，数组里的length是state 成员变量
  - 数组的长度定义之后就**不能改变** 
  - 数组的长度确定之后，里面所有元素的值会被设定为默认值
    - 对于数值类型的数组，默认值是0
    - 对于char类型，默认值是`\u0000`
    - 对于boolean类型，默认值是false
  - 数组可以在定义的时候直接赋予若干值，此时数组的长度会被自动设定为值的个数，但是下面这种方式是错误的
  
  ```java
  // 下面这种不行
  double[] list;
  list = {1, 2, 3, 4};
  
  // 这种可以
  double[] list = {1, 2, 3, 4};
  // 相当于
  double[] list = new double[4];
  list[0] = 1; // ...
  ```
  
  - Java数组和C++数组的**区别** 
    - Java的数组定义在**堆**上，C++中直接声明大小的数组分配在栈上，动态分配的数组在堆上
    - Java的[ ]运算符被会检查数组边界，防止下标溢出，并且没有指针运算
    - Java中命令行参数是`String[] args`，其中`args[0]`是第一个参数，**程序名没有存储**在args中需要在启动Java程序的时候就输入
    
  - 数组的遍历
    - 用下标i去遍历一个数组
    - 用`elementType value: arrayRefVar` 的形式来遍历
    
  - 数组的拷贝：`arraycopy(sourceArray, src_pos, targetArray, tar_pos, length);` 自带的处理方法
    - 或者用Arrays的copyOf函数
    
      ```java
      int[] copiedLuckyNumbers = Arrays.copyOf(fluckNumbers,luckyNumbers.length)；
      ```
    
         第2个参数是新数组的长度，这个方法通常用来增加数组的大小：
    
      ```java
      int[] copiedLuckyNumbers = Arrays.copyOf(luckNumbers,2 *luckyNumbers.length);
      ```
    
         如果数组元素是数值型，则多余的元素被赋值为0；若是布尔型，则赋值为false。
    
         相反，如果长度小于原始数组长度，则只拷贝前面的元素。
    
    - 在拷贝的过程中，如果被拷贝的数组存在多余的元素，则赋以默认值，如果小于原始数组长度，则只拷贝前面的元素
    
  - 数组作为方法的参数
    - Anonymous Array 匿名数组：在方法调用中的参数里直接写一个数组，没有用变量去引用，如`new int[]{1,2,3}`直接作为参数
    - 将数组作为参数的时候，数组作为一个引用传入，方法中改变数组的值将会影响到原来的数组，但是匿名数组没有变量名
    
  - 类对象的数组里放的是对象的**指针**，所以需要每个都new一下

###### 2.7.2 多维数组

- 二维数组的定义：`dataType[][] refVar;` 
  - 如何new：`new dataType[rowSize][columnSize]` 
  - Java的二维数组每一列大小可以不同，并不需要完全相同！**Ragged Arrays**
- 如何创建一个二维数组？以下四种写法中，只有第2行的是对的
  - 总结起来就是中括号可以放在前面也可以放在后面，但是**声明的时候不能直接指定数组的大小**
  - 数组的大小需要在new的时候指定，并且二维数组**一定要有行的数目** 

```Java
int a[3][2] = {{1,2}, {3,4}, {5,6}}; // 错，a后面没有具体行列值
int a[][] = new int[3][]; // 对
int[][] a = new int[][3]; // 错
int[][] a = new int[][]; // 错
```

