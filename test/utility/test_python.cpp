#include "../utility/python/python_caller.h"
#include "Python.h"



void callPython() {
	//�趨����ֵ
	int a = 0;
	int b = 6;



	//PyRun_SimpleString����ִ���������
	//����python3�Ĵ�ӡ���

	ofec::PythonCaller::initPythonFunction(ofec::g_working_dir + "/utility/python/instance/example");

	PyObject* pModule;
	PyObject* pFunction;
	PyObject* pArgs;
	PyObject* pRetValue;
	//�ڶ���������math_util�ű�
	pModule = PyImport_ImportModule("math_util");
	if (!pModule) {
		printf("import python failed1!!\n");
	}
	//�����������Ҷ�Ӧmath_test.py�е�def mainfunc(a,b)����
	pFunction = PyObject_GetAttrString(pModule, "mainfunc");
	if (!pFunction) {
		printf("get python function failed!!!\n");
	}
	//���Ĳ����½�python�е�tuple���󣨹���������
	pArgs = PyTuple_New(2);
	//PyTuple_SetItem(pArgs, 0, Py_BuildValue(""));
	PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", a));
	PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", b));
	//���岽�����ú���
	pRetValue = PyObject_CallObject(pFunction, pArgs);

	//�����������PyObject 
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


