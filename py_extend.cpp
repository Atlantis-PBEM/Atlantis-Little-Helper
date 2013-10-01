//#include "Python.h"
//
//static PyObject * spam_system(PyObject *self, PyObject *args)
//{
//    char *command;
//    int sts;
//
//    if (!PyArg_ParseTuple(args, "s", &command))
//        return NULL;
//    sts = system(command);
//    return Py_BuildValue("i", sts);
//}
//
//
//static PyMethodDef SpamMethods[] =
//{
//    {"system",  spam_system, METH_VARARGS,  "Execute a shell command."},
//    {NULL, NULL, 0, NULL}        /* Sentinel */
//};
//
//
//extern "C" void initspam(void)
//{
//    (void) Py_InitModule("spam", SpamMethods);
//}
//


/* Example of embedding Python in another program */

#include "Python.h"

void initxyzzy(void); /* Forward */

void dotest()
{
	/* Pass argv[0] to the Python interpreter */
	Py_SetProgramName("xyzzy");

	/* Initialize the Python interpreter.  Required. */
	Py_Initialize();

	/* Add a static module */
	initxyzzy();

	/* Define sys.argv.  It is up to the application if you
	   want this; you can also let it undefined (since the Python
	   code is generally not a main program it has no business
	   touching sys.argv...) */
  char * argv[1] = {"xyzzy"};
	PySys_SetArgv(1, argv);

	/* Do some application specific code */
	printf("Hello, brave new world\n\n");

	/* Execute some Python statements (in module __main__) */
	PyRun_SimpleString("import xyzzy");

	PyRun_SimpleString("import sys\n");
	PyRun_SimpleString("print sys.builtin_module_names\n");
	PyRun_SimpleString("print sys.modules.keys()\n");
	PyRun_SimpleString("print sys.executable\n");
	PyRun_SimpleString("print sys.argv\n");

	PyRun_SimpleString("print xyzzy.foo()\n");
	/* Note that you can call any public function of the Python
	   interpreter here, e.g. call_object(). */

	/* Some more application specific code */
	printf("\nGoodbye, cruel world\n");

	/* Exit, cleaning up the interpreter */
//	Py_Exit(0);
	/*NOTREACHED*/
}

/* A static module */

/* 'self' is not used */
extern "C" PyObject * xyzzy_foo(PyObject *self, PyObject* args)
{
	return PyInt_FromLong(42L);
}

PyMethodDef xyzzy_methods[] =
{
	{"foo",		xyzzy_foo,	METH_NOARGS,  "Return the meaning of everything."},
	{NULL,		NULL}		/* sentinel */
};

void initxyzzy(void)
{
	PyImport_AddModule("xyzzy");
	Py_InitModule("xyzzy", xyzzy_methods);
}

