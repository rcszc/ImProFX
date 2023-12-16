// framework_cpython_interface.

#ifndef _FRAMEWORK_CPYTHON_INFTERFACE_H
#define _FRAMEWORK_CPYTHON_INFTERFACE_H
#include <any>
#include "../improfx_log/framework_log.hpp"

// [2023_12_12](1): ImProFx脚本(CPython)接口.
namespace IFC_PYTHON {
	typedef void* BytecodeObject;

	typedef int64_t     ReturnInteger;
	typedef float       ReturnFloat;
	typedef std::string ReturnString;

	// cpython 解释器.
	class ImCPythonInterpreter {
	public:
		virtual bool InitCPythonInterpreter() = 0;
		virtual bool FreeCPythonInterpreter() = 0;
		// python script => bytecode.
		virtual BytecodeObject CompilePythonScript(std::string) = 0;
	};

	class ImCPythonLoggerFmt {
	public:
		// python function logger =return=> cpython object.
		virtual BytecodeObject ReceiveCPythonLog(BytecodeObject module, std::string func_name) = 0;

		// 获取Python函数对象输出格式化为框架日志.
		virtual std::string FrameworkFormatLog(BytecodeObject py_object) = 0;
	};

	class ImCPythonFunctionObject {
	public:
		// 构造(Python函数)参数.
		virtual void ConstructionParameters(std::vector<std::any> parameters) = 0;

		// Python函数返回参数.
		virtual ReturnInteger ReturnValueInt64(BytecodeObject py_object)  = 0;
		virtual ReturnFloat   ReturnValueFloat(BytecodeObject py_object)  = 0;
		virtual ReturnString  ReturnValueString(BytecodeObject py_object) = 0;
	};
}

#endif