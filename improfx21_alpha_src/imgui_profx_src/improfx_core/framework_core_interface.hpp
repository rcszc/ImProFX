// framework_core_interface.

#ifndef _FRAMEWORK_CORE_INTERFACE_H
#define _FRAMEWORK_CORE_INTERFACE_H

// framework start interface.
class INTERFACE_PROFX_START {
public:
	virtual bool    ProFxCoreInit() = 0;
	virtual bool    ProFxCoreFree() = 0;
	virtual int32_t ProFxCoreEventloop() = 0;
};

/*
* framework reflection interface. [2023_12_11]
* func: UserObj* ReflectionObjectFind(const char* UniqueName);
*/
class INTERFACE_REF_USERCLASS {
public:
	virtual void ReflectionObjectCreate(const char* ClassName, const char* UniqueName) = 0;
	virtual void ReflectionObjectDelete(const char* UniqueName) = 0;
};

#endif