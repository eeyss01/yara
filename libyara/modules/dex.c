#define _GNU_SOURCE

#include <yara/dex.h>

#include <yara/modules.h>
#include <yara/endian.h>
#include <yara/modules.h>
#include <yara/mem.h>

#define MODULE_NAME dex

// DEX File layout information: https://source.android.com/devices/tech/dalvik/dex-format
begin_declarations;

  declare_string("DEX_FILE_MAGIC_035");
  declare_string("DEX_FILE_MAGIC_036");
  declare_string("DEX_FILE_MAGIC_037");
  declare_string("DEX_FILE_MAGIC_038");

  declare_integer("ENDIAN_CONSTANT");
  declare_integer("REVERSE_ENDIAN_CONSTANT");
  declare_integer("NO_INDEX");
  declare_integer("ACC_PUBLIC");
  declare_integer("ACC_PRIVATE");
  declare_integer("ACC_PROTECTED");
  declare_integer("ACC_STATIC");
  declare_integer("ACC_FINAL");
  declare_integer("ACC_SYNCHRONIZED");
  declare_integer("ACC_VOLATILE");
  declare_integer("ACC_BRIDGE");
  declare_integer("ACC_TRANSIENT");
  declare_integer("ACC_VARARGS");
  declare_integer("ACC_NATIVE");
  declare_integer("ACC_INTERFACE");
  declare_integer("ACC_ABSTRACT");
  declare_integer("ACC_STRICT");
  declare_integer("ACC_SYNTHETIC");
  declare_integer("ACC_ANNOTATION");
  declare_integer("ACC_ENUM");
  declare_integer("ACC_CONSTRUCTOR");
  declare_integer("ACC_DECLARED_SYNCHRONIZED");

  declare_integer("TYPE_HEADER_ITEM");
  declare_integer("TYPE_STRING_ID_ITEM");
  declare_integer("TYPE_TYPE_ID_ITEM");
  declare_integer("TYPE_PROTO_ID_ITEM");
  declare_integer("TYPE_FIELD_ID_ITEM");
  declare_integer("TYPE_METHOD_ID_ITEM");
  declare_integer("TYPE_CLASS_DEF_ITEM");
  declare_integer("TYPE_CALL_SITE_ID_ITEM");
  declare_integer("TYPE_METHOD_HANDLE_ITEM");
  declare_integer("TYPE_MAP_LIST");
  declare_integer("TYPE_TYPE_LIST");
  declare_integer("TYPE_ANNOTATION_SET_REF_LIST");
  declare_integer("TYPE_ANNOTATION_SET_ITEM");
  declare_integer("TYPE_CLASS_DATA_ITEM");
  declare_integer("TYPE_CODE_ITEM");
  declare_integer("TYPE_STRING_DATA_ITEM");
  declare_integer("TYPE_DEBUG_INFO_ITEM");
  declare_integer("TYPE_ANNOTATION_ITEM");
  declare_integer("TYPE_ENCODED_ARRAY_ITEM");
  declare_integer("TYPE_ANNOTATIONS_DIRECTORY_ITEM");

  begin_struct("header");
    declare_string("magic");
    declare_integer("checksum");
    declare_string("signature");
    declare_integer("file_size");
    declare_integer("header_size");
    declare_integer("endian_tag");
    declare_integer("link_size");
    declare_integer("link_offset");
    declare_integer("map_offset");
    declare_integer("string_ids_size");
    declare_integer("string_ids_offset");
    declare_integer("type_ids_size");
    declare_integer("type_ids_offset");
    declare_integer("proto_ids_size");
    declare_integer("proto_ids_offset");
    declare_integer("field_ids_size");
    declare_integer("field_ids_offset");
    declare_integer("method_ids_size");
    declare_integer("method_ids_offset");
    declare_integer("class_defs_size");
    declare_integer("class_defs_offset");
    declare_integer("data_size");
    declare_integer("data_offset");
  end_struct("header");

  begin_struct_array("string_ids");
    declare_integer("offset");
    declare_integer("size");
    declare_string("value");
  end_struct_array("string_ids");

  begin_struct_array("type_ids");
    declare_integer("descriptor_idx");
  end_struct_array("type_ids");

  begin_struct_array("proto_ids");
    declare_integer("shorty_idx");
    declare_integer("return_type_idx");
    declare_integer("parameters_offset");
  end_struct_array("proto_ids");

  begin_struct_array("field_ids");
    declare_integer("class_idx");
    declare_integer("type_idx");
    declare_integer("name_idx");
  end_struct_array("field_ids");
  
  begin_struct_array("method_ids");
    declare_integer("class_idx");
    declare_integer("proto_idx");
    declare_integer("name_idx");
  end_struct_array("method_ids");

  begin_struct_array("class_defs");
     declare_integer("class_idx");
     declare_integer("access_flags");
     declare_integer("superclass_idx");
     declare_integer("interfaces_offset");
     declare_integer("source_file_idx");
     declare_integer("annotations_offset");
     declare_integer("class_data_offset");
     declare_integer("static_values_offset");
  end_struct_array("class_defs");

  begin_struct_array("class_data_item");
   declare_integer("static_fields_size");
   declare_integer("instance_fields_size");
   declare_integer("direct_methods_size");
   declare_integer("virtual_methods_size");
  end_struct_array("class_data_item");

  begin_struct("map_list");
    declare_integer("size");
    begin_struct_array("map_item");
      declare_integer("type");
      declare_integer("unused");
      declare_integer("size");
      declare_integer("offset");
    end_struct_array("map_item");
  end_struct("map_list");

  declare_integer("number_of_fields");
  begin_struct_array("field");
    declare_string("class_name");
    declare_string("name");
    declare_string("proto");

    declare_integer("field_idx_diff");
    declare_integer("access_flags");
  end_struct_array("field");

  declare_integer("number_of_methods");
  begin_struct_array("method");
    declare_string("class_name");
    declare_string("name");
    declare_string("proto");
    declare_integer("direct");
    declare_integer("virtual");
    
    declare_integer("method_idx_diff");
    declare_integer("access_flags");
    declare_integer("code_off");
    begin_struct("code_item");
      declare_integer("registers_size");
      declare_integer("ins_size");
      declare_integer("outs_size");
      declare_integer("tries_size");
      declare_integer("debug_info_off");
      declare_integer("insns_size");
      declare_string("insns");
      declare_integer("padding");
      begin_struct("tries");
      end_struct("tries");
      begin_struct_array("handlers");
      end_struct_array("handlers");
    end_struct("code_item");
  end_struct_array("method");

end_declarations;

// https://android.googlesource.com/platform/dalvik/+/android-4.4.2_r2/libdex/Leb128.cpp
int32_t read_uleb128(const uint8_t* pStream, uint32_t *size)
{
    const uint8_t* ptr = pStream;
    int32_t result = *(ptr++);
    *size = *size + 1;

    if (result > 0x7f) {
        int cur = *(ptr++);
        *size = *size + 1;
        result = (result & 0x7f) | ((cur & 0x7f) << 7);
        if (cur > 0x7f) {
            cur = *(ptr++);
            *size = *size + 1;
            result |= (cur & 0x7f) << 14;
            if (cur > 0x7f) {
                cur = *(ptr++);
                *size = *size + 1;
                result |= (cur & 0x7f) << 21;
                if (cur > 0x7f) {
                    /*
                     * Note: We don't check to see if cur is out of
                     * range here, meaning we tolerate garbage in the
                     * high four-order bits.
                     */
                    cur = *(ptr++);
                    *size = *size + 1;
                    result |= cur << 28;
                }
            }
        }
    }
    return result;
}


dex_header_t* dex_get_header(
    const uint8_t* data,
    size_t data_size)
{
  dex_header_t* dex_header;

  if (data_size < sizeof(dex_header_t))
    return NULL;

  // Check if we have a valid DEX file
  dex_header = (dex_header_t*) data;
  if (memcmp(dex_header->magic, DEX_FILE_MAGIC_035, 8) != 0 &&
      memcmp(dex_header->magic, DEX_FILE_MAGIC_036, 8) != 0 &&
      memcmp(dex_header->magic, DEX_FILE_MAGIC_037, 8) != 0 &&
      memcmp(dex_header->magic, DEX_FILE_MAGIC_038, 8) != 0) {
    return NULL;
  }

  return dex_header;
}

void dex_parse_header(dex_header_t*dex_header, YR_OBJECT* module_object)
{
  set_sized_string((char *)dex_header->magic, strnlen((char *)dex_header->magic,
                   8*sizeof(char)), module_object, "header.magic");
  set_integer(dex_header->checksum, module_object, "header.checksum");
  set_sized_string((char *)dex_header->signature,
                   strnlen((char *)dex_header->signature, 20*sizeof(char)),
                   module_object, "header.signature");
  set_integer(dex_header->file_size, module_object,
              "header.file_size");
  set_integer(dex_header->header_size, module_object,
              "header.header_size");
  set_integer(dex_header->endian_tag, module_object,
              "header.endian_tag");
  set_integer(dex_header->link_size, module_object,
              "header.link_size");
  set_integer(dex_header->link_offset, module_object,
              "header.link_offset");
  set_integer(dex_header->map_offset, module_object,
              "header.map_offset");
  set_integer(dex_header->string_ids_size, module_object,
              "header.string_ids_size");
  set_integer(dex_header->string_ids_offset, module_object,
              "header.string_ids_offset");
  set_integer(dex_header->type_ids_size, module_object,
              "header.type_ids_size");
  set_integer(dex_header->type_ids_offset, module_object,
              "header.type_ids_offset");
  set_integer(dex_header->proto_ids_size, module_object,
              "header.proto_ids_size");
  set_integer(dex_header->proto_ids_offset, module_object,
              "header.proto_ids_offset");
  set_integer(dex_header->field_ids_size, module_object,
              "header.field_ids_size");
  set_integer(dex_header->field_ids_offset, module_object,
              "header.field_ids_offset");
  set_integer(dex_header->method_ids_size, module_object,
              "header.method_ids_size");
  set_integer(dex_header->method_ids_offset, module_object,
              "header.method_ids_offset");
  set_integer(dex_header->class_defs_size, module_object,
              "header.class_defs_size");
  set_integer(dex_header->class_defs_offset, module_object,
              "header.class_defs_offset");
  set_integer(dex_header->data_size, module_object,
              "header.data_size");
  set_integer(dex_header->data_offset, module_object,
              "header.data_offset");
}

uint32_t load_encoded_field(
    DEX* dex,
    size_t start_offset,
    uint32_t *previous_field_idx,
    int index_encoded_field,
    int static_field,
    int instance_field)
{

  uint32_t current_size = 0;

  encoded_field_t encoded_field;

  encoded_field.field_idx_diff = (uint32_t)read_uleb128((
    dex->data + start_offset + current_size), &current_size);
  encoded_field.access_flags = (uint32_t)read_uleb128((
    dex->data + start_offset + current_size), &current_size);

  *previous_field_idx = encoded_field.field_idx_diff + *previous_field_idx;

  set_integer(encoded_field.field_idx_diff, dex->object,
              "field[%i].field_idx_diff", index_encoded_field);
  set_integer(encoded_field.access_flags, dex->object,
              "field[%i].access_flags", index_encoded_field);
  set_integer(static_field, dex->object,
              "field[%i].static", index_encoded_field);
  set_integer(instance_field, dex->object,
              "field[%i].instance", index_encoded_field);

  uint32_t name_idx = get_integer(dex->object,
    "field_ids[%i].name_idx", *previous_field_idx);
  SIZED_STRING* field_name = get_string(dex->object,
    "string_ids[%i].value", name_idx);
  set_sized_string(field_name->c_string, field_name->length, dex->object,
                   "field[%i].name", index_encoded_field);

  uint32_t class_idx = get_integer(dex->object,
    "field_ids[%i].class_idx", *previous_field_idx);
  uint32_t descriptor_idx = get_integer(dex->object,
    "type_ids[%i].descriptor_idx", class_idx);
  SIZED_STRING* class_name = get_string(dex->object,
    "string_ids[%i].value", descriptor_idx);
  set_sized_string(class_name->c_string, class_name->length, dex->object,
                   "field[%i].class_name", index_encoded_field);

  uint32_t type_idx = get_integer(dex->object,
    "field_ids[%i].type_idx", *previous_field_idx);
  uint32_t shorty_idx = get_integer(dex->object,
    "type_ids[%i].descriptor_idx", type_idx);
  SIZED_STRING* proto_name = get_string(dex->object,
    "string_ids[%i].value", shorty_idx);
  set_sized_string(proto_name->c_string, proto_name->length, dex->object, 
                   "field[%i].proto", index_encoded_field);

  return current_size;
}

uint32_t load_encoded_method(
    DEX* dex,
    size_t start_offset,
    uint32_t *previous_method_idx,
    int index_encoded_method,
    int direct_method,
    int virtual_method)
{
  uint32_t current_size = 0;

  encoded_method_t encoded_method;

  encoded_method.method_idx_diff = (uint32_t)read_uleb128(
    (dex->data + start_offset + current_size), &current_size);
  encoded_method.access_flags = (uint32_t)read_uleb128(
    (dex->data + start_offset + current_size), &current_size);
  encoded_method.code_off = (uint32_t)read_uleb128(
    (dex->data + start_offset + current_size), &current_size);

  *previous_method_idx = encoded_method.method_idx_diff + *previous_method_idx;

  set_integer(encoded_method.method_idx_diff, dex->object, 
              "method[%i].method_idx_diff", index_encoded_method);
  set_integer(encoded_method.access_flags, dex->object, 
              "method[%i].access_flags", index_encoded_method);
  set_integer(encoded_method.code_off, dex->object,
              "method[%i].code_off", index_encoded_method);
  set_integer(direct_method, dex->object,
              "method[%i].direct", index_encoded_method);
  set_integer(virtual_method, dex->object,
              "method[%i].virtual", index_encoded_method);

  uint32_t name_idx = get_integer(
    dex->object, "method_ids[%i].name_idx", *previous_method_idx);
  SIZED_STRING* method_name = get_string(
    dex->object,  "string_ids[%i].value", name_idx);
  set_sized_string(method_name->c_string, method_name->length, dex->object, 
                  "method[%i].name", index_encoded_method);

  uint32_t class_idx = get_integer(
    dex->object, "method_ids[%i].class_idx", *previous_method_idx);
  uint32_t descriptor_idx = get_integer(
    dex->object, "type_ids[%i].descriptor_idx", class_idx);
  SIZED_STRING* class_name = get_string(
    dex->object, "string_ids[%i].value", descriptor_idx);
  set_sized_string(class_name->c_string, class_name->length, dex->object,
                   "method[%i].class_name", index_encoded_method);

  uint32_t proto_idx = get_integer(
    dex->object, "method_ids[%i].proto_idx", *previous_method_idx);
  uint32_t shorty_idx = get_integer(
    dex->object, "proto_ids[%i].shorty_idx", proto_idx);
  SIZED_STRING* proto_name = get_string(
    dex->object, "string_ids[%i].value", shorty_idx);
  set_sized_string(proto_name->c_string, proto_name->length, dex->object,
                   "method[%i].proto", index_encoded_method);

  if (encoded_method.code_off != 0) {
    code_item_t *code_item = (code_item_t *) (
      dex->data + encoded_method.code_off);

    set_integer(code_item->registers_size, dex->object,
                "method[%i].code_item.registers_size", index_encoded_method);
    set_integer(code_item->ins_size, dex->object,
                "method[%i].code_item.ins_size", index_encoded_method);
    set_integer(code_item->outs_size, dex->object,
                "method[%i].code_item.outs_size", index_encoded_method);
    set_integer(code_item->tries_size, dex->object,
                "method[%i].code_item.tries_size", index_encoded_method);
    set_integer(code_item->debug_info_off, dex->object,
                "method[%i].code_item.debug_info_off", index_encoded_method);
    set_integer(code_item->insns_size, dex->object,
                "method[%i].code_item.insns_size", index_encoded_method);

    set_sized_string(
      (const char *)(dex->data + encoded_method.code_off + sizeof(code_item_t)), 
      code_item->insns_size * 2, 
      dex->object, "method[%i].code_item.insns", index_encoded_method);
  }

  return current_size;
}

void dex_parse(
    DEX* dex,
    size_t base_address)
{
  dex_header_t* dex_header;

  uint32_t uleb128_size = 0;
  uint32_t index_class_data_item = 0;
  uint32_t index_encoded_method = 0;
  uint32_t index_encoded_field = 0;
  int i = 0;
  
  if (!struct_fits_in_dex(dex, dex->data, dex_header_t))
    return;

  dex_parse_header(dex->header, dex->object);

  dex_header = dex->header;

  if (!fits_in_dex(dex, dex->data + dex_header->string_ids_offset,
                   dex_header->string_ids_size * sizeof(string_id_item_t)))
    return;
  // Get information about the String ID section
  for(i = 0; i < dex_header->string_ids_size; i++) {
    string_id_item_t *string_id_item = (string_id_item_t *) (
      dex->data + dex_header->string_ids_offset + i * sizeof(string_id_item_t));
    uint32_t value = (uint32_t)read_uleb128(
      (dex->data + string_id_item->string_data_offset), &uleb128_size);

    set_integer(string_id_item->string_data_offset, dex->object,
                "string_ids[%i].offset", i);
    set_integer(string_id_item->string_data_offset, dex->object,
                "string_ids[%i].size", value);
    set_sized_string((const char *)((
                     dex->data + string_id_item->string_data_offset + 1)), 
                     value,
                     dex->object, "string_ids[%i].value", i);
  }

  if (!fits_in_dex(dex, dex->data + dex_header->type_ids_offset,
                   dex_header->type_ids_size * sizeof(type_id_item_t)))
    return;
  // Get information about the Type ID section
  for(i = 0; i < dex_header->type_ids_size; i++) {
    type_id_item_t *type_id_item = (type_id_item_t *) (
      dex->data + dex_header->type_ids_offset + i * sizeof(type_id_item_t));
    set_integer(type_id_item->descriptor_idx, dex->object,
                "type_ids[%i].descriptor_idx", i);
  }

  if (!fits_in_dex(dex, dex->data + dex_header->proto_ids_offset,
                   dex_header->proto_ids_size * sizeof(proto_id_item_t)))
    return;
  // Get information about the Proto ID section
  for(i = 0; i < dex_header->proto_ids_size; i++) {
    proto_id_item_t *proto_id_item = (proto_id_item_t *) (
      dex->data + dex_header->proto_ids_offset + i * sizeof(proto_id_item_t));
    set_integer(proto_id_item->shorty_idx, dex->object,
                "proto_ids[%i].shorty_idx", i);
    set_integer(proto_id_item->return_type_idx, dex->object,
                "proto_ids[%i].return_type_idx", i);
    set_integer(proto_id_item->parameters_offset, dex->object,
                "proto_ids[%i].parameters_offset", i);
  }

  if (!fits_in_dex(dex, dex->data + dex_header->field_ids_offset,
                   dex_header->field_ids_size * sizeof(field_id_item_t)))
    return;
  // Get information about the Field ID section
  for(i = 0; i < dex_header->field_ids_size; i++) {
    field_id_item_t *field_id_item = (field_id_item_t *) (
      dex->data + dex_header->field_ids_offset + i * sizeof(field_id_item_t));
    set_integer(field_id_item->class_idx, dex->object,
                "field_ids[%i].class_idx", i);
    set_integer(field_id_item->type_idx, dex->object,
                "field_ids[%i].type_idx", i);
    set_integer(field_id_item->name_idx, dex->object,
                "field_ids[%i].name_idx", i);
  }

  if (!fits_in_dex(dex, dex->data + dex_header->method_ids_offset,
                   dex_header->method_ids_size * sizeof(method_id_item_t)))
    return;
  // Get information about the Method ID section
  for(i = 0; i < dex_header->method_ids_size; i++) {
    method_id_item_t *method_id_item = (method_id_item_t *) (
      dex->data + dex_header->method_ids_offset + i * sizeof(method_id_item_t));
    set_integer(method_id_item->class_idx, dex->object,
                "method_ids[%i].class_idx", i);
    set_integer(method_id_item->proto_idx, dex->object,
                "method_ids[%i].proto_idx", i);
    set_integer(method_id_item->name_idx, dex->object,
                "method_ids[%i].name_idx", i);
  }

  // Get information about the Map List ID section
  if (dex_header->map_offset != 0 && 
      fits_in_dex(dex, dex->data + dex_header->map_offset, sizeof(uint32_t))) {

    uint32_t *map_list_size = (uint32_t *) (dex->data + dex_header->map_offset);

    set_integer(*map_list_size, dex->object, "map_list.size");
    
    if (!fits_in_dex(dex, dex->data + dex_header->map_offset,
                     sizeof(uint32_t) + *map_list_size * sizeof(map_item_t)))
      return;

    for(i = 0; i < *map_list_size; i++) {
      map_item_t *map_item = (map_item_t *) (
        dex->data + dex_header->map_offset + 
        sizeof(uint32_t) + i * sizeof(map_item_t));

      set_integer(map_item->type, dex->object,
                  "map_list.map_item[%d].type", i);
      set_integer(map_item->unused, dex->object, 
                  "map_list.map_item[%d].unused", i);
      set_integer(map_item->size, dex->object,
                  "map_list.map_item[%d].size", i);
      set_integer(map_item->offset, dex->object,
                  "map_list.map_item[%d].offset", i);
    }
  }

  if (!fits_in_dex(dex, dex->data + dex_header->class_defs_offset,
                   dex_header->class_defs_size * sizeof(class_id_item_t)))
    return;
  // Get information about the Class ID section
  for(i = 0; i < dex_header->class_defs_size; i++) {
    class_id_item_t *class_id_item = (class_id_item_t *) (
      dex->data + dex_header->class_defs_offset + i * sizeof(class_id_item_t));
    set_integer(class_id_item->class_idx, dex->object,
                "class_defs[%i].class_idx", i);
    set_integer(class_id_item->access_flags, dex->object, 
                "class_defs[%i].access_flags", i);
    set_integer(class_id_item->super_class_idx, dex->object,
                "class_defs[%i].super_class_idx", i);
    set_integer(class_id_item->interfaces_off, dex->object, 
                "class_defs[%i].interfaces_off", i);
    set_integer(class_id_item->source_file_idx, dex->object, 
                "class_defs[%i].source_file_idx", i);
    set_integer(class_id_item->annotations_offset, dex->object,
                "class_defs[%i].annotations_offset", i);
    set_integer(class_id_item->class_data_offset, dex->object,
                "class_defs[%i].class_data_off", i);
    set_integer(class_id_item->static_values_offset, dex->object,
                "class_defs[%i].static_values_offset", i);

    if (class_id_item->class_data_offset != 0) {
      class_data_item_t class_data_item;

      if (!fits_in_dex(dex, dex->data + class_id_item->class_data_offset,
                       4 * sizeof(uint32_t)))
        return;

      uleb128_size = 0;
      class_data_item.static_fields_size = (uint32_t)read_uleb128(
        (dex->data + class_id_item->class_data_offset),
        &uleb128_size);
      class_data_item.instance_fields_size = (uint32_t)read_uleb128(
        (dex->data + class_id_item->class_data_offset + uleb128_size),
        &uleb128_size);
      class_data_item.direct_methods_size = (uint32_t)read_uleb128(
        (dex->data + class_id_item->class_data_offset + uleb128_size),
        &uleb128_size);
      class_data_item.virtual_methods_size = (uint32_t)read_uleb128(
        (dex->data + class_id_item->class_data_offset + uleb128_size),
        &uleb128_size);

      set_integer(
        class_data_item.static_fields_size, dex->object, 
        "class_data_item[%i].static_fields_size", index_class_data_item);
      set_integer(
        class_data_item.instance_fields_size, dex->object,
        "class_data_item[%i].instance_fields_size", index_class_data_item);
      set_integer(
        class_data_item.direct_methods_size, dex->object,
        "class_data_item[%i].direct_methods_size", index_class_data_item);
      set_integer(
        class_data_item.virtual_methods_size, dex->object,
        "class_data_item[%i].virtual_methods_size", index_class_data_item);

      int j;
      uint32_t previous_field_idx = 0;
      for(j = 0; j < class_data_item.static_fields_size; j++) {
        uleb128_size += load_encoded_field(
          dex,
          class_id_item->class_data_offset + uleb128_size,
          &previous_field_idx,
          index_encoded_field,
          1, 0);

        index_encoded_field += 1;
      }

      previous_field_idx = 0;
      for(j = 0; j < class_data_item.instance_fields_size; j++) {
        uleb128_size += load_encoded_field(
          dex,
          class_id_item->class_data_offset + uleb128_size,
          &previous_field_idx,
          index_encoded_field,
          0, 1);

        index_encoded_field += 1;
      }

      uint32_t previous_method_idx = 0;
      for(j = 0; j < class_data_item.direct_methods_size; j++) {
        uleb128_size += load_encoded_method(
          dex,
          class_id_item->class_data_offset + uleb128_size,
          &previous_method_idx,
          index_encoded_method,
          1, 0);
        index_encoded_method += 1;
      }

      previous_method_idx = 0;
      for(j = 0; j < class_data_item.virtual_methods_size; j++) {
        uleb128_size += load_encoded_method(
          dex,
          class_id_item->class_data_offset + uleb128_size,
          &previous_method_idx,
          index_encoded_method,
          0, 1);
        index_encoded_method += 1;
      }


      index_class_data_item++;
    }
  }

  set_integer(index_encoded_method, dex->object, "number_of_methods");
  set_integer(index_encoded_field, dex->object, "number_of_fields");
}

int module_initialize(YR_MODULE* module)
{
  return ERROR_SUCCESS;
}

int module_finalize(YR_MODULE* module)
{
  return ERROR_SUCCESS;
}

int module_load(
    YR_SCAN_CONTEXT* context,
    YR_OBJECT* module_object,
    void* module_data,
    size_t module_data_size)
{
  YR_MEMORY_BLOCK* block;
  YR_MEMORY_BLOCK_ITERATOR* iterator = context->iterator;

  dex_header_t* dex_header;

  set_string(DEX_FILE_MAGIC_035, module_object, "DEX_FILE_MAGIC_035");
  set_string(DEX_FILE_MAGIC_036, module_object, "DEX_FILE_MAGIC_036");
  set_string(DEX_FILE_MAGIC_037, module_object, "DEX_FILE_MAGIC_037");
  set_string(DEX_FILE_MAGIC_038, module_object, "DEX_FILE_MAGIC_038");


  set_integer(0x12345678, module_object, "ENDIAN_CONSTANT");
  set_integer(0x78563412, module_object, "REVERSE_ENDIAN_CONSTANT");

  set_integer(0xffffffff, module_object, "NO_INDEX");
  set_integer(0x1, module_object, "ACC_PUBLIC");
  set_integer(0x2, module_object, "ACC_PRIVATE");
  set_integer(0x4, module_object, "ACC_PROTECTED");
  set_integer(0x8, module_object, "ACC_STATIC");
  set_integer(0x10, module_object, "ACC_FINAL");
  set_integer(0x20, module_object, "ACC_SYNCHRONIZED");
  set_integer(0x40, module_object, "ACC_VOLATILE");
  set_integer(0x40, module_object, "ACC_BRIDGE");
  set_integer(0x80, module_object, "ACC_TRANSIENT");
  set_integer(0x80, module_object, "ACC_VARARGS");
  set_integer(0x100, module_object, "ACC_NATIVE");
  set_integer(0x200, module_object, "ACC_INTERFACE");
  set_integer(0x400, module_object, "ACC_ABSTRACT");
  set_integer(0x800, module_object, "ACC_STRICT");
  set_integer(0x1000, module_object, "ACC_SYNTHETIC");
  set_integer(0x2000, module_object, "ACC_ANNOTATION");
  set_integer(0x4000, module_object, "ACC_ENUM");
  set_integer(0x10000, module_object, "ACC_CONSTRUCTOR");
  set_integer(0x20000, module_object, "ACC_DECLARED_SYNCHRONIZED");

  set_integer(0x0000, module_object, "TYPE_HEADER_ITEM");
  set_integer(0x0001, module_object, "TYPE_STRING_ID_ITEM");
  set_integer(0x0002, module_object, "TYPE_TYPE_ID_ITEM");
  set_integer(0x0003, module_object, "TYPE_PROTO_ID_ITEM");
  set_integer(0x0004, module_object, "TYPE_FIELD_ID_ITEM");
  set_integer(0x0005, module_object, "TYPE_METHOD_ID_ITEM");
  set_integer(0x0006, module_object, "TYPE_CLASS_DEF_ITEM");
  set_integer(0x0007, module_object, "TYPE_CALL_SITE_ID_ITEM");
  set_integer(0x0008, module_object, "TYPE_METHOD_HANDLE_ITEM");
  set_integer(0x1000, module_object, "TYPE_MAP_LIST");
  set_integer(0x1001, module_object, "TYPE_TYPE_LIST");
  set_integer(0x1002, module_object, "TYPE_ANNOTATION_SET_REF_LIST");
  set_integer(0x1003, module_object, "TYPE_ANNOTATION_SET_ITEM");
  set_integer(0x2000, module_object, "TYPE_CLASS_DATA_ITEM");
  set_integer(0x2001, module_object, "TYPE_CODE_ITEM");
  set_integer(0x2002, module_object, "TYPE_STRING_DATA_ITEM");
  set_integer(0x2003, module_object, "TYPE_DEBUG_INFO_ITEM");
  set_integer(0x2004, module_object, "TYPE_ANNOTATION_ITEM");
  set_integer(0x2005, module_object, "TYPE_ENCODED_ARRAY_ITEM");
  set_integer(0x2006, module_object, "TYPE_ANNOTATIONS_DIRECTORY_ITEM");


  foreach_memory_block(iterator, block)
  {
    const uint8_t* block_data = block->fetch_data(block);

    if (block_data == NULL)
      continue;

    dex_header = dex_get_header(block_data, block->size);
    if (dex_header != NULL)
    {
      DEX* dex = (DEX*) yr_malloc(sizeof(DEX));

      if (dex == NULL)
        return ERROR_INSUFFICIENT_MEMORY;

      dex->data = block_data;
      dex->data_size = block->size;
      dex->object = module_object;
      dex->header = dex_header;

      module_object->data = dex;

      dex_parse(dex, block->base);
      break;
    }
  }

  return ERROR_SUCCESS;
}

int module_unload(YR_OBJECT* module_object)
{
  DEX* dex = (DEX *) module_object->data;

  if (dex == NULL)
    return ERROR_SUCCESS;

  yr_free(dex);

  return ERROR_SUCCESS;
}

#undef MODULE_NAME