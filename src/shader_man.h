#ifndef INCLUDEshader_man_
#define INCLUDEshader_man_


#include <raylib.h>
typedef unsigned long long ShaderHandle;

ShaderHandle load_shader(const char *path_vs, const char *path_fs, const char *name);
ShaderHandle load_shader_from_string(const char *str_vs, const char *str_fs, const char *name);

ShaderHandle get_shader_handle(const char *name);
Shader get_shader_by_handle(ShaderHandle shader_handle);

void add_shader_parameter(ShaderHandle shader_handle, const char *name, ShaderUniformDataType value_type);

#define set_shader_value(shader_handle, name, value, type) \
  do { type tmp = value; set_shader_value_ptr(shader_handle, name, &tmp); } while(0)

void set_shader_value_ptr(ShaderHandle shader_handle, const char *name, void *new_value);
void *get_shader_value(ShaderHandle shader_handle, const char *name);

void unload_shader_by_handle(ShaderHandle shader_handle);
void unload_all_shaders();


#endif // INCLUDEshader_man_
