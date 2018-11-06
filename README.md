# json
A simple and easy json library for converting json to c++ class and converting c++ class to json.
***
## c++ class to json
将C++类转化为JSON格式的字符串时，需要在C++类定义相关字段时用reflect模板类进行包装。  
refelect模板类目前支持的类型有
```
bool
char 
unsigned char
short
unsigned short
int
unsigned int
long
unsigned long
long long
unsigned long long
std::string
std::vector
```
下面是用reflect包装字段的一些例子
```
struct peple
{
	// 所有需要json序列化的字段需要用reflect模板类进行包装
	reflect<int> age;	
	reflect<string> name;
	peple()
		: age("json:age")
		, name("json:name")
	{

	}
};
```
```
struct family
{
	// 没用reflect模板类进行包装的字段不参与序列化
	int id;
	reflect<string> addr;
	reflect<peple> father;
	reflect<peple> mother;
	reflect<vector<peple>> children;

	// reflect模板类的构造函数需要传入string类型的标签。
	// 标签可以认为是该变量的扩展信息，用来字段映射、对象关系映射等。
	// 标签格式为"xxx:xxx"
	// 如json库指定的标签格式为"json:xxx"，orm库指定的标签格式为"orm:xxx"
	family()
		: addr("json:addr orm:address")
		, father("json:father")
		, mother("json:mother")
		, children("json:children")
	{

	}
};
```
接下来用上面定义的peple、family来演示一下该如何将C++类转化为JSON。我们先请出王大锤的一家：
```
	// 构造测试数据
	// reflect已重载操作符=，可以直接使用=赋值
	peple father;father.age = 40;father.name = "王大锤";
	peple mother;mother.age = 35;mother.name = "柳青青";
	peple child1;child1.age = 3;child1.name = "王旺财";
	peple child2;child2.age = 4;child2.name = "王来福";

	// 构造测试数据
	family myfamily;
	myfamily.id = 0;
	myfamily.addr = "地球村中国";
	myfamily.father = father;
	myfamily.mother = mother;

	// 获取reflect包装的数据需要使用重载操作符()
	myfamily.children().push_back(child1);
	myfamily.children().push_back(child2); 
```
对构造好的测试数据进行json编码，我们只需要调用一行代码：
```
	string jsonStr = json::Marshal(myfamily, true);
```
只需调用json类的Marshal静态方法，传入王大锤的一家就可以得到json格式的字符串了。以下是jsonStr的打印结果:
```
{
    "addr": "地球村中国",
    "father": {
        "age": 40,
        "name": "王大锤"
    },
    "mother": {
        "age": 35,
        "name": "柳青青"
    },
    "children": [
        {
            "age": 3,
            "name": "王旺财"
        },
        {
            "age": 4,
            "name": "王来福"
        }
    ]
}
```
***
## json to c++ class
将json转化为C++类，同样只需调用一行代码:
```
	family myfamily2;
	json::Unmarshal(jsonStr, myfamily2)
```
json类的Unmarshal静态方法，提供json格式字符串到C++类的功能。当然这个C++类的成员变量必须是用reflect模板类包装过的。
最后我们来打印一下myfamily2的信息：
```
	// 测试打印json解码结果
	cout << "-------------json解码:--------------" << endl;
	cout << "id:" << myfamily2.id 
		<< "\t// 这个id是未初始化的，因为没用reflect包装，不参与json编解码" << endl;
	cout << "addr:" << myfamily2.addr() << endl;
	cout << "father:" << endl;
	cout << "\tage:" << myfamily2.father().age() << endl;
	cout << "\tname:" << myfamily2.father().name() << endl;
	cout << "mother:" << endl;
	cout << "\tage:" << myfamily2.mother().age() << endl;
	cout << "\tname:" << myfamily2.mother().name() << endl;
	cout << "children:" << endl;
	for (auto iter = myfamily2.children().begin(); iter != myfamily2.children().end(); ++iter)
	{
		cout << "\tage:" << iter->age() << endl;
		cout << "\tname:" << iter->name() << endl;
		cout << "\t---------------" << endl;
	}
```
输出结果如下：
```
-------------json解码:--------------
id:3339688	// 这个id是未初始化的，因为没用reflect包装，不参与json编解码
addr:地球村中国
father:
	age:40
	name:王大锤
mother:
	age:35
	name:柳青青
children:
	age:3
	name:王旺财
	---------------
	age:4
	name:王来福
	---------------
```
***
## 关于工作原理
reflect模板类提供了运行时的反射功能：标签、类型、内存布局。
reflect在构造函数中会扫描被包装类型实例的内存，利用C++的RTTI信息构建该类型的内存布局模型。在JSON编解码的时候利用类型的内存模型进行编解码。  
JSON编解码部分是采用boost库提供的相关功能。   
更多实现细节请自行查看相关源码。
***
## 写在最后
本库的宗旨是简单、简单、再简单。就个人而言，所接触的所有C++ JSON库抛去性能不谈，光易用性方面都不是很好。C++由于没有反射功能，我所接触的这些库实现只能大量使用宏，或者大量编写额外代码来实现JSON到C++类，及C++类到JSON的功能，这样高层业务代码非常臃肿，不像其他语言那样方便。  
本库从Golang语言中的json库获得了大量灵感，于是萌生了写这个库的想法。当然由于需要扫描内存建立内存布局，性能方面是不及Golang的。上面的测试例子，编码耗时大概0.08ms，比Golang的效率低了大概20多倍，有兴趣的朋友可以跟其他语言的json库做个性能对比。但是由于为了追求易用性，我决定这些性能牺牲都是值得的。  
最后，感谢王大锤的一家，最近一个月下班时间有你们的陪伴，我不再孤单，谢谢！






