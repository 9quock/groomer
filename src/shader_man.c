#include <string.h>
#include "shader_man.h"
#include "../nob.h"

typedef struct ShaderValue {
  char *name;
  int loc;
  void *cached_value;
  ShaderUniformDataType value_type;
} ShaderValue;
typedef struct ShaderValues {
  ShaderValue *items;
  size_t count;
  size_t capacity;
} ShaderValues;
typedef struct ShaderMeta {
  char *name;
  Shader shader;
  ShaderValues values;
} ShaderMeta;
typedef struct Shaders {
  ShaderMeta *items;
  size_t count;
  size_t capacity;
} Shaders;


static Shaders shaders = {0};

ShaderHandle load_shader(const char *path_vs, const char *path_fs, const char *name) {
  Shader shader = LoadShader(path_vs, path_fs);

  ShaderMeta shader_meta = {0};
  shader_meta.name = strdup(name);
  shader_meta.shader = shader;

  nob_da_append(&shaders, shader_meta);

  return shaders.count;
}
ShaderHandle load_shader_from_string(const char *str_vs, const char *str_fs, const char *name) {
  Shader shader = LoadShaderFromMemory(str_vs, str_fs);

  ShaderMeta shader_meta = {0};
  shader_meta.name = strdup(name);
  shader_meta.shader = shader;

  nob_da_append(&shaders, shader_meta);

  return shaders.count;
}

// returns 0 on fail
ShaderHandle get_shader_handle(const char *name) {
  nob_da_foreach(ShaderMeta, shader, &shaders) { 
    if(strcmp(shader->name, name) == 0) {
      size_t index = shader - shaders.items;
      return index+1;
    }
  }
  return 0;
}
Shader get_shader_by_handle(ShaderHandle shader_handle) {
  return shaders.items[shader_handle-1].shader;
}

size_t get_value_type_size(ShaderUniformDataType type) {
  switch(type) {
    case SHADER_UNIFORM_FLOAT:     return sizeof(float);
    case SHADER_UNIFORM_VEC2:      return sizeof(float)*2;
    case SHADER_UNIFORM_VEC3:      return sizeof(float)*3;
    case SHADER_UNIFORM_VEC4:      return sizeof(float)*4;
    case SHADER_UNIFORM_INT:       return sizeof(int);
    case SHADER_UNIFORM_IVEC2:     return sizeof(int)*2;
    case SHADER_UNIFORM_IVEC3:     return sizeof(int)*3;
    case SHADER_UNIFORM_IVEC4:     return sizeof(int)*4;
    case SHADER_UNIFORM_UINT:      return sizeof(unsigned int);
    case SHADER_UNIFORM_UIVEC2:    return sizeof(unsigned int)*2;
    case SHADER_UNIFORM_UIVEC3:    return sizeof(unsigned int)*3;
    case SHADER_UNIFORM_UIVEC4:    return sizeof(unsigned int)*4;
    case SHADER_UNIFORM_SAMPLER2D: return sizeof(Texture);
  }
  return 0;
}

void add_shader_parameter(ShaderHandle shader_handle, const char *name, ShaderUniformDataType value_type) {
  if(shader_handle == 0) return;

  Shader shader = get_shader_by_handle(shader_handle);

  ShaderValue shader_value = {0};
  shader_value.name = strdup(name);
  shader_value.loc = GetShaderLocation(shader, shader_value.name);
  shader_value.cached_value = malloc(get_value_type_size(value_type));
  shader_value.value_type = value_type;

  nob_da_append(&shaders.items[shader_handle-1].values, shader_value);
}

void set_shader_value_ptr(ShaderHandle shader_handle, const char *name, void *new_value) {
  if(shader_handle == 0) return;

  nob_da_foreach(ShaderValue, value, &shaders.items[shader_handle-1].values) { 
    if(strcmp(value->name, name) == 0) {
      size_t value_size = get_value_type_size(value->value_type);
      if(memcmp(new_value, value->cached_value, value_size) != 0) {
        memcpy(value->cached_value, new_value, value_size);
        SetShaderValue(get_shader_by_handle(shader_handle), value->loc, value->cached_value, value->value_type);
      }
      return;
    }
  }
}
void *get_shader_value(ShaderHandle shader_handle, const char *name) {
  if(shader_handle == 0) return NULL;

  nob_da_foreach(ShaderValue, value, &shaders.items[shader_handle-1].values) { 
    if(strcmp(value->name, name) == 0) {
      return value->cached_value;
    }
  }

  return NULL;
}

void unload_shader_by_handle(ShaderHandle shader_handle) {
  if(shader_handle == 0) return;

  Shader shader = get_shader_by_handle(shader_handle);

  nob_da_foreach(ShaderValue, value, &shaders.items[shader_handle-1].values) { 
    free(value->name);
    free(value->cached_value);
  }
  nob_da_free(shaders.items[shader_handle-1].values);

  free(shaders.items[shader_handle-1].name);
  UnloadShader(shader);

  nob_da_remove_unordered(&shaders, shader_handle-1);
}
void unload_all_shaders() {
  nob_da_foreach(ShaderMeta, shader, &shaders) { 
    size_t index = shader - shaders.items;
    unload_shader_by_handle(index+1);
  }

  nob_da_free(shaders);

  shaders.items = NULL;
  shaders.count = 0;
  shaders.capacity = 0;
}
