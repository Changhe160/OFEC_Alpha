#include "../utility/python/python_caller.h"
#include "Python.h"



void callPython() {
	//设定参数值
	int a = 0;
	int b = 6;



	//PyRun_SimpleString（）执行命令语句
	//测试python3的打印语句

	ofec::PythonCaller::initPythonFunction(ofec::g_working_dir + "/utility/python/instance/example");

	PyObject* pModule;
	PyObject* pFunction;
	PyObject* pArgs;
	PyObject* pRetValue;
	//第二步：调用math_util脚本
	pModule = PyImport_ImportModule("math_util");
	if (!pModule) {
		printf("import python failed1!!\n");
	}
	//第三步：查找对应math_test.py中的def mainfunc(a,b)函数
	pFunction = PyObject_GetAttrString(pModule, "mainfunc");
	if (!pFunction) {
		printf("get python function failed!!!\n");
	}
	//第四步：新建python中的tuple对象（构建参数）
	pArgs = PyTuple_New(2);
	//PyTuple_SetItem(pArgs, 0, Py_BuildValue(""));
	PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", a));
	PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", b));
	//第五步：调用函数
	pRetValue = PyObject_CallObject(pFunction, pArgs);

	//第六步：清空PyObject 
	//Py_DECREF(pModule);
	//Py_DECREF(pFunction);
	//Py_DECREF(pArgs);
	//Py_DECREF(pRetValue);

	if (!pModule) {
		printf("import python failed!!\n");
	}
}



TEST_CASE("utility_python", "[python]")
{
	callPython();
}


