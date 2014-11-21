/*
Copyright 2014 Google Inc. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stdlib.h>
#include <stdio.h>

#include <Python.h>

#include "libjsonnet.h"

struct module_state {
				PyObject *error;
};

const char* PyString_AsString(PyObject* obj){
				return PyUnicode_AS_DATA(obj);
}

PyObject* PyString_FromString(const char* ptr) {
				return PyUnicode_FromString(ptr);
}

static PyObject* evaluate_file(PyObject* self, PyObject* args, PyObject *keywds)
{
				const char *filename, *out;
				unsigned max_stack = 500, gc_min_objects = 1000, max_trace = 20;
				double gc_growth_trigger = 2;
				int debug_ast = 0, error;
				PyObject *env = NULL;
				struct JsonnetVM *vm;
				static char *kwlist[] = { "filename", "max_stack", "gc_min_objects", "gc_growth_trigger", "env", "debug_ast", "max_trace", NULL };

				(void)self;

				if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|IIdOiI", kwlist,
								&filename,
								&max_stack, &gc_min_objects, &gc_growth_trigger, &env, &debug_ast, &max_trace)) {
								return NULL;
				}

				vm = jsonnet_make();
				jsonnet_max_stack(vm, max_stack);
				jsonnet_gc_min_objects(vm, gc_min_objects);
				jsonnet_max_trace(vm, max_trace);
				jsonnet_gc_growth_trigger(vm, gc_growth_trigger);
				jsonnet_debug_ast(vm, debug_ast);
				if (env != NULL) {
								PyObject *key, *val;
								Py_ssize_t pos = 0;

								while (PyDict_Next(env, &pos, &key, &val)) {
												const char *key_ = PyString_AsString(key);
												if (key_ == NULL) {
																jsonnet_destroy(vm);
																return NULL;
												}
												const char *val_ = PyString_AsString(val);
												if (val_ == NULL) {
																jsonnet_destroy(vm);
																return NULL;
												}
												jsonnet_ext_var(vm, key_, val_);
								}
				}

				out = jsonnet_evaluate_file(vm, filename, &error);
				if (error) {
								PyErr_SetString(PyExc_RuntimeError, out);
								jsonnet_cleanup_string(vm, out);
								jsonnet_destroy(vm);
								return NULL;
				}
				else {
								PyObject *ret = PyString_FromString(out);
								jsonnet_cleanup_string(vm, out);
								jsonnet_destroy(vm);
								return ret;
				}
}

static PyObject* evaluate_snippet(PyObject* self, PyObject* args, PyObject *keywds)
{
				const char *filename, *src, *out;
				unsigned max_stack = 500, gc_min_objects = 1000, max_trace = 20;
				double gc_growth_trigger = 2;
				int debug_ast = 0, error;
				PyObject *env = NULL;
				struct JsonnetVM *vm;
				static char *kwlist[] = {
								"filename",
								"src",
								"max_stack",
								"gc_min_objects",
								"gc_growth_trigger",
								"env",
								"debug_ast",
								"max_trace",
								NULL
				};

				(void)self;

				if (!PyArg_ParseTupleAndKeywords(args, keywds, "ss|IIdOiI", kwlist,
								&filename, &src,
								&max_stack, &gc_min_objects, &gc_growth_trigger, &env, &debug_ast, &max_trace)) {
								return NULL;
				}

				vm = jsonnet_make();
				jsonnet_max_stack(vm, max_stack);
				jsonnet_gc_min_objects(vm, gc_min_objects);
				jsonnet_max_trace(vm, max_trace);
				jsonnet_gc_growth_trigger(vm, gc_growth_trigger);
				jsonnet_debug_ast(vm, debug_ast);
				if (env != NULL) {
								PyObject *key, *val;
								Py_ssize_t pos = 0;

								while (PyDict_Next(env, &pos, &key, &val)) {
												const char *key_ = PyString_AsString(key);
												if (key_ == NULL) {
																jsonnet_destroy(vm);
																return NULL;
												}
												const char *val_ = PyString_AsString(val);
												if (val_ == NULL) {
																jsonnet_destroy(vm);
																return NULL;
												}
												jsonnet_ext_var(vm, key_, val_);
								}
				}

				out = jsonnet_evaluate_snippet(vm, filename, src, &error);
				if (error) {
								PyErr_SetString(PyExc_RuntimeError, out);
								jsonnet_cleanup_string(vm, out);
								jsonnet_destroy(vm);
								return NULL;
				}
				else {
								PyObject *ret = PyString_FromString(out);
								jsonnet_cleanup_string(vm, out);
								jsonnet_destroy(vm);
								return ret;
				}
}

static PyMethodDef module_methods[] = {
								{ "evaluate_file", (PyCFunction)evaluate_file, METH_VARARGS | METH_KEYWORDS,
								"Interpret the given Jsonnet file." },
								{ "evaluate_snippet", (PyCFunction)evaluate_snippet, METH_VARARGS | METH_KEYWORDS,
								"Interpret the given Jsonnet code." },
								{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef moduledef = {
				PyModuleDef_HEAD_INIT,
				"_jsonnet",
				NULL,
				sizeof(struct module_state),
				module_methods,
				NULL,
				NULL,
				NULL,
				NULL
};

PyMODINIT_FUNC PyInit_jsonnet(void)
{
				PyModule_Create(&moduledef);
}

