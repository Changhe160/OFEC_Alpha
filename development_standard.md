## 命名规范

标识符内单词尽可能用全程（即不要省略字母），其他规范参考 [Google C++ Style](https://google.github.io/styleguide/cppguide.html)

|标识符类型|规范|示例|
|-|-|-|
|文件名|小写，下划线连接|`free_peaks.h`|
|局部变量|小写，下划线连接|`number_variables`|
|全局变量|前缀g_，小写，下划线连接|`g_working_directory`|
|函数|驼峰法，首字母小写|`customizeFileName()`|
|类、结构体、枚举类型|驼峰法|`FreePeaks`|
|数据成员|前缀m_，小写，下划线连接|`m_number_objectives`|
|静态数据成员|前缀ms_，小写，下划线连接|`ms_registered`|
|常量、宏|大写，下划线连接|`OFEC_PI`|
|枚举成员|前缀k，驼峰法|`kMinimize`|
|命名空间|小写，下划线连接|`cec_2005`|
|模板类型参数|前缀T，驼峰法|`TValueType`|


## 其他书写规范

- 代码写在命名空间`ofec`内
- 在头文件中不要`using`任何命名空间
- 使用前缀为`OFEC_`的头文件保护宏
- 函数、类、控制语句的左花括号尽量不换行
- 派生类重写虚函数时务必添加`override`声明
- 指针和引用参数类型中，`*`和`&`贴靠参数名（例外是作为返回值类型时，由于没有参数名，`*`和`&`贴靠参数类型）


## 内容规范

- 添加算法和问题实例时，源文件须与头文件同名，并在头文件中参考以下示例添加注释文本，文本内容为json格式的注册信息（包括字符串名、类标识符、问题标签），以供OFEC Filter软件识别新添加的问题或算法示例
- 把问题或算法实例中从外界给数据成员赋值（通过`m_input_parameters->add(...)`）的代码放在构造函数中
- 把问题或算法实例中数据成员的初始化步骤放在`initialize_()`中，并记得首先调用父类的`initialize_()`方法
```
/********* Begin Register Information **********
{
	"name": "Canonical-DE",
	"identifier": "Canonical_DE",
	"problem tags": [ "MMOP", "ConOP", "SOP", "GOP" ]
}
*********** End Register Information **********/
```

