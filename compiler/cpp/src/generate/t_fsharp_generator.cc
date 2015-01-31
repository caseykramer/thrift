/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 * Contains some contributions under the Thrift Software License.
 * Please see doc/old-thrift-license.txt in the Thrift distribution for
 * details.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include "t_oop_generator.h"
#include "platform.h"
#include "version.h"
#include "parse/t_type.h"

using namespace std;

/**
 * A subclass of std::ofstream that includes indenting functionality.
 */
class t_fs_ofstream : public std::ofstream {
  private:
    int indent_;

  public:
    t_fs_ofstream() : std::ofstream(), indent_(0) { }
    explicit t_fs_ofstream(const char* filename, ios_base::openmode mode = ios_base::out, int indent = 0) :
      std::ofstream(filename, mode), indent_(indent) { }

  t_fs_ofstream& indent() {
    for (int i = 0; i < indent_; ++i) { *this << "    "; }
    return *this;
  }

  void indent_up() { indent_++; }
  void indent_down() { indent_--; }
};

class t_fsharp_generator : public t_oop_generator {
	public:
    t_fsharp_generator(
        t_program* program,
        const std::map<std::string, std::string>& parsed_options,
        const std::string& option_string)
      : t_oop_generator(program)
    {
      (void) option_string;

      std::map<std::string, std::string>::const_iterator iter;
      out_dir_base_ = "gen-fsharp";
    }
    void init_generator();
    void close_generator();

    void generate_consts(std::vector<t_const*> consts);

    void generate_typedef	(t_typedef* ttypedef);
    void generate_enum		(t_enum* tenum);
    void generate_struct	(t_struct* tstruct);
    void generate_xception	(t_struct* txception);
    void generate_service	(t_service* tservice);
    void generate_property	(t_fs_ofstream& out, t_field* tfield, bool isPublic, bool generateIsset);
    void generate_fsharp_property(t_fs_ofstream& out, t_field* tfield, bool isPublic, bool includeIsset=true, std::string fieldPrefix = "");
    void print_const_value	(t_fs_ofstream& out, std::string name, t_type* type, t_const_value* value, bool in_static, bool defval=false, bool wrapIsset=false);
    std::string render_const_value(t_fs_ofstream& out, std::string name, t_type* type, t_const_value* value);
	std::string render_fsharp_type(t_type* type);
	void print_const_constructor(t_fs_ofstream& out, std::vector<t_const*> consts);
    void print_const_def_value(t_fs_ofstream& out, std::string name, t_type* type, t_const_value* value);

    void generate_fsharp_struct(t_struct* tstruct);
	void generate_fsharp_xception(t_struct* txception);
    void generate_fsharp_struct_definition(t_fs_ofstream& out, t_struct* tstruct, bool in_class=false, bool is_result=false);
    void generate_fsharp_wcffault(t_fs_ofstream& out, t_struct* tstruct);
    void generate_fsharp_struct_reader(t_fs_ofstream& out, t_struct* tstruct);
    void generate_fsharp_struct_result_writer(t_fs_ofstream& out, t_struct* tstruct);
    void generate_fsharp_struct_writer(t_fs_ofstream& out, t_struct* tstruct);
    void generate_fsharp_struct_tostring(t_fs_ofstream& out, t_struct* tstruct);

    void generate_function_helpers(t_function* tfunction);
    void generate_service_interface (t_service* tservice);
    void generate_service_helpers (t_service* tservice);
    void generate_service_client (t_service* tservice);
    void generate_service_server (t_service* tservice);
    void generate_process_function (t_service* tservice, t_function* function);	

    void generate_deserialize_field (t_fs_ofstream& out, t_field* tfield, std::string prefix="");
	void generate_deserialize_field_inline (t_fs_ofstream& out, t_field* tfield);
    void generate_deserialize_struct (t_fs_ofstream& out, t_struct* tstruct, std::string prefix="");
	void generate_deserialize_struct_inline (t_fs_ofstream& out, t_struct* tstruct);
    void generate_deserialize_container (t_fs_ofstream& out, t_type* ttype, std::string prefix="");
	void generate_deserialize_container_inline (t_fs_ofstream& out, t_type* ttype);
    void generate_deserialize_set_element (t_fs_ofstream& out, t_set* tset, std::string prefix="");
	void generate_deserialize_set_element_inline (t_fs_ofstream& out, t_set* tset);
    void generate_deserialize_map_element (t_fs_ofstream& out, t_map* tmap, std::string prefix="");
	void generate_deserialize_map_element_inline (t_fs_ofstream& out, t_map* tmap);
    void generate_deserialize_list_element (t_fs_ofstream& out, t_list* list, std::string prefix="");
	void generate_deserialize_list_element_inline (t_fs_ofstream& out, t_list* list);
    void generate_serialize_field (t_fs_ofstream& out, t_field* tfield, std::string prefix="");
    void generate_serialize_type(t_fs_ofstream& out, t_type* ttype, std::string name);
    void generate_serialize_value(t_fs_ofstream& out, t_field* tfield, std::string name);
	void generate_serialize_struct (t_fs_ofstream& out, t_struct* tstruct, std::string prefix="");
    void generate_serialize_container (t_fs_ofstream& out, t_type* ttype, std::string prefix="");    
    void generate_serialize_set_element (t_fs_ofstream& out, t_set* tmap, std::string iter);
    void generate_serialize_list_element (t_fs_ofstream& out, t_list* tlist, std::string iter);
	
    void generate_fsharp_doc (t_fs_ofstream& out, t_field*    field);
    void generate_fsharp_doc (t_fs_ofstream& out, t_doc*      tdoc);
    void generate_fsharp_doc (t_fs_ofstream& out, t_function* tdoc);
    void generate_fsharp_docstring_comment (t_fs_ofstream &out, string contents);

    std::string generate_fsharp_namespace ();    

    std::string fsharp_type_opens();
    std::string fsharp_thrift_opens();

    std::string type_name(t_type* ttype, bool in_countainer=false, bool in_init=false);
    std::string base_type_name(t_base_type* tbase, bool in_container=false);
    std::string declare_field(t_field* tfield, bool init=false, std::string prefix="");
    std::string function_signature_async_begin(t_function* tfunction, std::string prefix = "");
    std::string function_signature_async_end(t_function* tfunction, std::string prefix = "");
    std::string function_signature_async_ctp(t_function* tfunction, std::string prefix = "");
    std::string function_signature(t_function* tfunction, std::string prefix="");
    std::string argument_list(t_struct* tstruct);
    std::string type_to_enum(t_type* ttype);
    std::string prop_name(t_field* tfield);
    std::string get_enum_class_name(t_type* type);

    bool type_can_be_null(t_type* ttype) {
        return false;
    }

	
  private:
	std::string make_property_name(std::string s);
	std::string make_field_name(std::string s);
	void generate_isset_type(t_fs_ofstream& out);
    std::string namespace_name_;
    std::string namespace_dir_;
    t_fs_ofstream f_types_;
	t_fs_ofstream f_consts_;
	t_fs_ofstream f_service_;
};

std::string t_fsharp_generator::make_property_name(std::string in) {
    in[0] = toupper(in[0]);
    return in;
}

std::string t_fsharp_generator::prop_name(t_field* tfield) {
	return make_property_name(tfield->get_name());
}

std::string t_fsharp_generator::make_field_name(std::string in) {
	in[0] = tolower(in[0]);
	return "_"+in;
}

void t_fsharp_generator::init_generator() {
  MKDIR(get_out_dir().c_str());
  namespace_name_ = program_->get_namespace("fsharp");

  string dir = namespace_name_;
  string subdir = get_out_dir().c_str();
  string::size_type loc;

  while ((loc = dir.find(".")) != string::npos) {
    subdir = subdir + "/" + dir.substr(0, loc);
    MKDIR(subdir.c_str());
    dir = dir.substr(loc + 1);
  }
  if (dir.size() > 0) {
    subdir = subdir + "/" + dir;
    MKDIR(subdir.c_str());
  }

  namespace_dir_ = subdir;
  string f_types_name = subdir + "/types.fs";
  f_types_.open(f_types_name.c_str());
  string f_consts_name = subdir + "/consts.fs";
  f_consts_.open(f_consts_name.c_str());
  string f_service_name = subdir + "/service.fs";
  f_service_.open(f_service_name.c_str());
  
  f_types_ << generate_fsharp_namespace() << endl
				<< fsharp_type_opens()
				<< fsharp_thrift_opens() << endl << endl;
  generate_isset_type(f_types_);
				
  f_consts_ << generate_fsharp_namespace() << endl << endl
				<< fsharp_type_opens() << endl
				<< fsharp_thrift_opens() << endl << endl;
  f_service_ << generate_fsharp_namespace()
				<< fsharp_type_opens()
				<< fsharp_thrift_opens() << endl << endl;
}

void t_fsharp_generator::generate_isset_type(t_fs_ofstream& out) {
	out.indent() << "module Helpers =" << endl;
	out.indent_up();
    // start readMap
	out.indent() << "let inline readMap (iprot:TProtocol) f =" << endl;
	out.indent_up();
	out.indent() << "let _map = iprot.ReadMapBegin()" << endl;
	out.indent() << "let result = f _map.Count" << endl;
	out.indent() << "iprot.ReadMapEnd()" << endl;
	out.indent() << "result" << endl << endl;
	out.indent_down();
    // end readMap
    // start readSet
    out.indent() << "let inline readSet (iprot:TProtocol) f = " << endl;
    out.indent_up();
    out.indent() << "let _set = iprot.ReadSetBegin()" << endl;
    out.indent() << "let result = f _set.Count" << endl;
    out.indent() << "iprot.ReadSetEnd()" << endl;
    out.indent() << "result" << endl << endl;
    out.indent_down();
    // end readSet
    // start readList
    out.indent() << "let inline readList (iprot:TProtocol) f = " << endl;
    out.indent_up();
    out.indent() << "let _list = iprot.ReadListBegin()" << endl;
    out.indent() << "let result = f _list.Count" << endl;
    out.indent() << "iprot.ReadListEnd()" << endl;
    out.indent() << "result" << endl << endl;
    out.indent_down();
    // end readList
    // start readStruct
    out.indent() << "let inline readStruct (iprot:TProtocol) (tbase:#TBase) = " << endl;
    out.indent_up();
    out.indent() << "tbase.Read(iprot)" << endl;
    out.indent() << "tbase" << endl;
    out.indent_down();
    out << endl << endl;
    // end readStruct
    // start writeStruct
    out.indent() << "let inline writeStruct (oprot:TProtocol) (tbase:#TBase) = " << endl;
    out.indent_up();
    out.indent() << "tbase.Write(oprot)" << endl << endl;
    out.indent_down();
    // end writeStruct
    out.indent_down();

	out.indent() << "type Isset<'a> = " << endl;
	out.indent_up();
	out.indent() << "| NotSet" << endl;
	out.indent() << "| IsSet of 'a" << endl;
	out.indent_down();
}

std::string t_fsharp_generator::generate_fsharp_namespace() {
  if (!namespace_name_.empty()) {
    return string() + "namespace " + namespace_name_ + "\n";    
  }
  return string();
}

string t_fsharp_generator::fsharp_type_opens() {
  return string() +
    "open System.Runtime.Serialization\n" +
	"open System.Collections.Generic\n" +
    "open System.Collections\n" +
    "open Thrift.Collections\n" +
	"open System.Text\n" +
    "open System.IO\n" +
	"open System\n" +
    "open Thrift\n";
}

string t_fsharp_generator::fsharp_thrift_opens() {
  return string() +
    "open Thrift.Transport\n" +
	"open Thrift.Protocol\n";
    
}

void t_fsharp_generator::close_generator() { }
void t_fsharp_generator::generate_typedef(t_typedef* ttypedef) {  
    
}

string render_fsharp_type(t_type* type) {
  while (type->is_typedef()) {
    type = ((t_typedef*)type)->get_type();
  }

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      return "unit";
    case t_base_type::TYPE_STRING:
      return "string";
    case t_base_type::TYPE_BOOL:
      return "bool";
    case t_base_type::TYPE_BYTE:
      return "byte";
    case t_base_type::TYPE_I16:
      return "int16";
    case t_base_type::TYPE_I32:
      return "int";
    case t_base_type::TYPE_I64:
      return "int64";
    case t_base_type::TYPE_DOUBLE:
      return "float";
    }
  } else if (type->is_enum()) {
    return ((t_enum*)type)->get_name();
  } else if (type->is_struct() || type->is_xception()) {
    return ((t_struct*)type)->get_name();
  } else if (type->is_map()) {
    t_type* ktype = ((t_map*)type)->get_key_type();
    t_type* vtype = ((t_map*)type)->get_val_type();
    return "Map<"+render_fsharp_type(ktype)+","+render_fsharp_type(vtype)+">";
  } else if (type->is_set()) {
    t_type* etype = ((t_set*)type)->get_elem_type();
    return render_fsharp_type(etype)+" set";
  } else if (type->is_list()) {
    t_type* etype = ((t_list*)type)->get_elem_type();
    return render_fsharp_type(etype)+" list";
  }

  throw "INVALID TYPE IN type_to_enum: " + type->get_name();
}

void t_fsharp_generator::generate_enum(t_enum* tenum) {
  indent(f_types_) <<
    "type " << tenum->get_name() << "\n";
  f_types_.indent_up();

  vector<t_enum_value*> constants = tenum->get_constants();
  vector<t_enum_value*>::iterator c_iter;
  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
	generate_fsharp_doc(f_types_, *c_iter);

    int value = (*c_iter)->get_value();
    indent(f_types_) << "| " << (*c_iter)->get_name() << " = " << value << endl;
  }

  f_types_.indent_down();

}

void t_fsharp_generator::generate_consts(std::vector<t_const*> consts) {
  if (consts.empty()){
    return;
  }
  f_consts_.indent_up();
  f_consts_.indent() << "module Constants = " << endl;
  f_consts_.indent_up();

  vector<t_const*>::iterator c_iter;
  for (c_iter = consts.begin(); c_iter != consts.end(); ++c_iter) {
	generate_fsharp_doc(f_consts_, (*c_iter));    
	print_const_value(f_consts_, (*c_iter)->get_name(), (*c_iter)->get_type(), (*c_iter)->get_value(), false);
        
  }
  f_consts_.indent_down();
  f_consts_.indent_down();
  
}

void t_fsharp_generator::print_const_def_value(t_fs_ofstream& out, string name, t_type* type, t_const_value* value)
{
  if (type->is_struct() || type->is_xception()) {
    const vector<t_field*>& fields = ((t_struct*)type)->get_members();
    vector<t_field*>::const_iterator f_iter;
    const map<t_const_value*, t_const_value*>& val = value->get_map();
    map<t_const_value*, t_const_value*>::const_iterator v_iter;
	out << "(";
    bool first = true;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      t_type* field_type = NULL;
      for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
        if ((*f_iter)->get_name() == v_iter->first->get_string()) {
          field_type = (*f_iter)->get_type();
        }
      }
      if (field_type == NULL) {
        throw "type error: " + type->get_name() + " has no field " + v_iter->first->get_string();
      }
      string val = render_const_value(out, name, field_type, v_iter->second);

      out << make_property_name(v_iter->first->get_string()) << " = (IsSet " << val << ")";
      if (first) {
          first = false;
      }
      else {
          out << ", ";
      }

    }
	out << ")";
  } else if (type->is_map()) {
	out << "[";
	t_type* ktype = ((t_map*)type)->get_key_type();
    t_type* vtype = ((t_map*)type)->get_val_type();
    const map<t_const_value*, t_const_value*>& val = value->get_map();
    map<t_const_value*, t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      string key = render_const_value(out, name, ktype, v_iter->first);
      string val = render_const_value(out, name, vtype, v_iter->second);
      out << key << " , " << val << ";";
    }
	out << "] |> Map.ofList";
  } else if (type->is_list() || type->is_set()) {
    t_type* etype;
    if (type->is_list()) {
      etype = ((t_list*)type)->get_elem_type();
    } else {
      etype = ((t_set*)type)->get_elem_type();
    }

    const vector<t_const_value*>& val = value->get_list();
    vector<t_const_value*>::const_iterator v_iter;
    for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
      string val = render_const_value(out, name, etype, *v_iter);
      out.indent() << name << ".Add(" << val << ");" << endl;
    }
  }
}


void t_fsharp_generator::print_const_value(t_fs_ofstream& out, string name, t_type* type, t_const_value* value, bool in_static, bool defval, bool wrapIsset) {
  if (wrapIsset) {
	  out << "IsSet (";
  }
  if (type->is_base_type()) {
    string v2 = render_const_value(out, name, type, value);
    out << v2;    
  } else if (type->is_enum()) {
    out << "enum<" << type_name(type, false, true) << ">" << value->get_integer();
  } else if (type->is_struct() || type->is_xception()) {
    out << type_name(type);
  }

  if (defval && !type->is_base_type() && !type->is_enum()) {
    print_const_def_value(out, name, type, value);
  }

}

std::string t_fsharp_generator::render_const_value(t_fs_ofstream& out, string name, t_type* type, t_const_value* value) {
  (void) name;
  std::ostringstream render;

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
      case t_base_type::TYPE_STRING:
        render << '"' << get_escaped_string(value) << '"';
        break;
      case t_base_type::TYPE_BOOL:
        render << ((value->get_integer() > 0) ? "true" : "false");
        break;
      case t_base_type::TYPE_BYTE:
		render << ((value->get_integer())) << "u";
		break;
      case t_base_type::TYPE_I16:
		render << value->get_integer() << "s";
		break;
      case t_base_type::TYPE_I32:
		render << value->get_integer();
		break;
      case t_base_type::TYPE_I64:
        render << value->get_integer() << "L";
        break;
      case t_base_type::TYPE_DOUBLE:
        if (value->get_type() == t_const_value::CV_INTEGER) {
          render << value->get_integer()<< ".";
        } else {
          render << value->get_double();
        }
        break;
      default:
        throw "compiler error: no const of base type " + tbase;
    }
  } else if (type->is_enum()) {
    render << "enum<" << type->get_name() << "> " << value->get_integer();
  } else {
    string t = tmp("tmp");
    print_const_value(out, t, type, value, true, true, true);
    render << t;
  }

  return render.str();
}

void t_fsharp_generator::generate_struct(t_struct* tstruct) {
  generate_fsharp_struct(tstruct);
}

void t_fsharp_generator::generate_xception(t_struct* txception) {
  generate_fsharp_xception(txception);
}

void t_fsharp_generator::generate_fsharp_struct(t_struct* tstruct) {
  generate_fsharp_struct_definition(f_types_,tstruct);  
}

void t_fsharp_generator::generate_fsharp_xception(t_struct* txception) {
  generate_fsharp_struct_definition(f_types_,txception);
}

void t_fsharp_generator::generate_fsharp_struct_definition(t_fs_ofstream &out, t_struct* tstruct, bool in_class, bool is_result) {

  out << endl;

  generate_fsharp_doc(out, tstruct);

  out.indent() << "type " << tstruct->get_name() << "() = " << endl;
  out.indent_up();
  const vector<t_field*>& members = tstruct->get_members();
  vector<t_field*>::const_iterator m_iter;

  //make auto-properties
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
	t_type* t = (*m_iter)->get_type();
	std::string name = capitalize((*m_iter)->get_name());
	out.indent() << "member val " << name << " = ";
	if ((*m_iter)->get_value() != NULL) {	  
      print_const_value(out, name, t, (*m_iter)->get_value(), true, true, true);
      out << ")";
    } else {
	  out << "NotSet";
	}
	out << " with get,set";
	out << endl;
  }  
  out.indent() << "interface TBase with" << endl;
  out.indent_up();
  generate_fsharp_struct_reader(out, tstruct);
  out << endl;
  if (is_result) {
    generate_fsharp_struct_result_writer(out, tstruct);
  } else {
    generate_fsharp_struct_writer(out, tstruct);
  }
  out << endl;
  out.indent_down();
  generate_fsharp_struct_tostring(out, tstruct);
  out.indent_down();
  out << endl;

}

void t_fsharp_generator::generate_fsharp_struct_reader(t_fs_ofstream& out, t_struct* tstruct) {
  out.indent() <<
    "member x.Read (iprot:TProtocol) = " << endl;
  out.indent_up();

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  // Create inline funciton for processing fields
  out.indent() << "let rec doRead() = " << endl;
  out.indent_up();
  out.indent() << "let field:TField = iprot.ReadFieldBegin()" << endl;
  out.indent() << "match field.Type with" << endl;
  out.indent() << "| TType.Stop -> ()";  
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
	out << endl;
    out.indent() << "| _ when field.ID  = " << (*f_iter)->get_key() << "s && field.Type = " << type_to_enum((*f_iter)->get_type()) << " -> " << endl;
    out.indent_up();
    out.indent();
	generate_deserialize_field(out, *f_iter,"x.");
	out.indent_down();
  }
  out.indent() << "| _ -> TProtocolUtil.Skip(iprot, field.Type)" << endl << endl;
  out.indent() << "iprot.ReadFieldEnd()" << endl;
  out.indent() << "doRead()" << endl << endl;
  out.indent_down();

  out.indent() << "iprot.ReadStructBegin() |> ignore" << endl;
  out.indent() << "doRead()" << endl;
  out.indent() << "iprot.ReadStructEnd()" << endl;

  out.indent_down();
}

void t_fsharp_generator::generate_fsharp_struct_writer(t_fs_ofstream& out, t_struct* tstruct) {
  out.indent() << "member x.Write(oprot:TProtocol) =" << endl;
  out.indent_up();

  string name = tstruct->get_name();
  const vector<t_field*>& fields = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator f_iter;

  out.indent() <<
    "let mutable struc:TStruct = TStruct(\"" << name << "\")" << endl;
  out.indent() <<
    "oprot.WriteStructBegin(struc);" << endl;

  if (fields.size() > 0) {
    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
		out.indent() << "match " << "x." << make_property_name((*f_iter)->get_name()) << " with" << endl;
		bool null_allowed = type_can_be_null((*f_iter)->get_type());
		if (null_allowed) {
		  out.indent() << "| null " << endl;          
		}
		out.indent() << "| NotSet -> ignore()" << endl;
		out.indent() << "| IsSet v -> " << endl;
		out.indent_up();
		out.indent() << "let mutable field:TField = TField()" << endl;
    

		out.indent() << "field.Name <- \"" << (*f_iter)->get_name() << "\"" << endl;
		out.indent() << "field.Type <- " << type_to_enum((*f_iter)->get_type()) << endl;
		out.indent() << "field.ID <- " << (*f_iter)->get_key() << "s" << endl << endl;
		out.indent() << "oprot.WriteFieldBegin(field)" << endl;

		generate_serialize_value(out, *f_iter,"v");

		out.indent() << "oprot.WriteFieldEnd()" << endl;
        out.indent_down();
	  }
      
      out << endl;
      out.indent() << "oprot.WriteFieldStop()" << endl;
  }
  out.indent() << "oprot.WriteStructEnd()" << endl;
  out.indent_down();

  out.indent() << endl;
}

void t_fsharp_generator::generate_fsharp_struct_result_writer(t_fs_ofstream& out, t_struct* tstruct) {
  out.indent() <<
    "member x.Write(oprot:TProtocol) =" << endl;
  indent_up();

  string name = tstruct->get_name();
  const vector<t_field*>& fields = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator f_iter;

  out.indent() <<
    "let struc:TStruct = TStruct(\"" << name << "\")" << endl;
  out.indent() <<
    "oprot.WriteStructBegin(struc)" << endl;

  if (fields.size() > 0) {
    out.indent() << "let mutable field:TField = TField();" << endl;
    bool first = true;
    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      if (first) {
        first = false;
        out <<
          endl << indent() << "if ";
      } else {
        out << " elif ";
      }

      out.indent() << "(this.__isset." << (*f_iter)->get_name() << ")" << endl;
	  indent_up();
	  out << "then ";
      

      bool null_allowed = type_can_be_null((*f_iter)->get_type());
      if (null_allowed) {
        out.indent() << "if (" << prop_name(*f_iter) << " != null)" << endl;
		indent_up();
		out << "then ";        
      }

      out.indent() <<
        "field.Name <- \"" << prop_name(*f_iter) << "\"" << endl;
      out.indent() <<
        "field.Type <- " << type_to_enum((*f_iter)->get_type()) << endl;
      out.indent() <<
        "field.ID <- " << (*f_iter)->get_key() << endl;
      out.indent() <<
        "oprot.WriteFieldBegin(field)" << endl;

      generate_serialize_field(out, *f_iter);

      out.indent() <<
        "oprot.WriteFieldEnd()" << endl;

      if (null_allowed) {
        indent_down();
        out.indent() << endl;
      }

      indent_down();
      out.indent();
    }
  }

  out <<
    endl <<
    indent() << "oprot.WriteFieldStop()" << endl <<
    indent() << "oprot.WriteStructEnd()" << endl;

  indent_down();

  out.indent() << endl << endl;
}

void t_fsharp_generator::generate_fsharp_struct_tostring(t_fs_ofstream& out, t_struct* tstruct) {
  out.indent() <<
    "override x.ToString():string =" << endl;
  out.indent_up();

  out.indent() <<
    "let sb:StringBuilder = StringBuilder(\"" << tstruct->get_name() << "(\");" << endl;

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  bool first = true;

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
      out.indent() <<
        "sb.Append(\"" << prop_name((*f_iter)) << ": \") |> ignore" << endl;
    } else {
      out.indent() <<
        "sb.Append(\"," << prop_name((*f_iter)) << ": \") |> ignore" << endl;
    }
    t_type* ttype = (*f_iter)->get_type();
    if (ttype->is_xception() || ttype->is_struct()) {
      out.indent() <<
        "sb.Append(" << "x." << prop_name((*f_iter))  << ".ToString()) |> ignore" << endl;
    } else {
      out.indent() <<
        "sb.Append(" << "x." << prop_name((*f_iter))  << ") |> ignore" << endl;
    }
  }

  out.indent() << "sb.Append(\")\") |> ignore" << endl;
  out.indent() << "sb.ToString()" << endl;

  out.indent_down();
}

void t_fsharp_generator::generate_service(t_service* tservice) {
  f_service_.indent() <<    "module " << service_name_ << " =" << endl;
  f_service_.indent_up();
  generate_service_helpers(tservice);
  generate_service_interface(tservice);
  generate_service_client(tservice);
  generate_service_server(tservice);
  
  f_service_.indent_down();

}

void t_fsharp_generator::generate_service_interface(t_service* tservice) {
  string extends = "";
  string extends_iface = "";
  if (tservice->get_extends() != NULL) {
    extends = type_name(tservice->get_extends());
    extends_iface = "interface " + extends + ".Iface";
  }

  generate_fsharp_doc(f_service_, tservice);

  f_service_.indent() << "type Iface = " << endl;
  f_service_.indent() << extends_iface;

  f_service_.indent_up();
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter)
  {
	generate_fsharp_doc(f_service_, *f_iter);

    f_service_.indent() <<
      function_signature(*f_iter) << endl;    
  }
  f_service_.indent_down();
  f_service_.indent() << endl << endl;
}

void t_fsharp_generator::generate_service_helpers(t_service* tservice) {
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_struct* ts = (*f_iter)->get_arglist();
    generate_fsharp_struct_definition(f_service_, ts, false, true);
    generate_function_helpers(*f_iter);
  }
}

void t_fsharp_generator::generate_service_client(t_service* tservice) {
  string extends = "";
  string extends_client = "";
  if (tservice->get_extends() != NULL) {
    extends = type_name(tservice->get_extends());
    extends_client = "inherit " + extends + ".Client(iprot,oprot)";
  }

  generate_fsharp_doc(f_service_, tservice);

  indent(f_service_) << "type Client(mutable iprot:TProtocol,mutable oprot:TProtocol) = " << endl;
  indent(f_service_) << extends_client << endl;

  indent_up();
	indent(f_service_) <<
		"new(TProtocol prot) = Client(prot, prot)" << endl;		
	indent(f_service_) << 
		"new() = Client(null,null)" << endl;
  
  indent_up();
  
  f_service_ << endl;

  if (extends.empty()) {
    f_service_ <<
      indent() << "let mutable seqid_:int = 0" << endl << endl;
  }
  f_service_ << indent() << "interface IFace with";
  scope_up(f_service_);
  f_service_ << indent() << "member x.InputProtocol:TProtocol" << endl <<
  indent() << indent() << "with get() = iprot" << endl <<
  indent() << indent() << "and set(v) = iprot <- v" << endl;

  f_service_ << indent() << "member x.OutputProtocol:TProtocol" << endl <<
  indent() << indent() << "with get() = oprot" << endl <<
  indent() << indent() << "and set(v) = oprot <- v" << endl;
  f_service_ << endl << endl;
  
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    string funname = (*f_iter)->get_name();

    indent(f_service_) << endl;
	
    generate_fsharp_doc(f_service_, *f_iter);
    indent(f_service_) <<
      "member " << function_signature(*f_iter) << endl;
    scope_up(f_service_);
    
    // Send
    t_function send_function(g_type_void,
        string("send_") + (*f_iter)->get_name(),
        (*f_iter)->get_arglist());

    string argsname = (*f_iter)->get_name() + "_args";

    scope_up(f_service_);

    f_service_ <<
      indent() << "oprot_.WriteMessageBegin(TMessage(\"" << funname << "\", TMessageType.Call, seqid_));" << endl <<
      indent() << argsname << " args = " << argsname << "();" << endl;
  }
	/*
	std::vector<t_field*>& fields = arg_struct->get_members();
    for (fld_iter = fields.begin(); fld_iter != fields.end(); ++fld_iter) {
      f_service_ <<
        indent() << "args." << prop_name(*fld_iter) << " <- " << (*fld_iter)->get_name() << endl;
    }

    f_service_ <<
      indent() << "args.Write(oprot_)" << endl <<
      indent() << "oprot_.WriteMessageEnd()" << endl;;

    scope_down(f_service_);
    f_service_ << endl;

    if (!(*f_iter)->is_oneway()) {
      string resultname = (*f_iter)->get_name() + "_result";

      t_struct noargs(program_);
      t_function recv_function((*f_iter)->get_returntype(),
          string("recv_") + (*f_iter)->get_name(),
          &noargs,
          (*f_iter)->get_xceptions());
      indent(f_service_) <<
        "member " << function_signature(&recv_function) << endl;
      scope_up(f_service_);

      f_service_ <<
        indent() << "let msg:TMessage = iprot_.ReadMessageBegin()" << endl <<
        indent() << "if (msg.Type = TMessageType.Exception)" << endl;
      indent_up();
      f_service_ << "then " << endl
        indent() << indent() << "let x:TApplicationException = TApplicationException.Read(iprot_)" << endl <<
        indent() << indent() << "iprot_.ReadMessageEnd()" << endl <<
        indent() << indent() << "raise x" << endl;
      indent_down();
      f_service_ <<
        indent() << "else" << endl <<
        indent() << resultname << "let result = " << resultname << "();" << endl <<
        indent() << "result.Read(iprot_)" << endl <<
        indent() << "iprot_.ReadMessageEnd()" << endl;
	    
      if (!(*f_iter)->get_returntype()->is_void()) {
        f_service_ <<
          indent() << "if result.__isset.success then" << endl <<
          indent() << "  result.Success" << endl <<
          indent() << endl;
      }

      t_struct *xs = (*f_iter)->get_xceptions();

      const std::vector<t_field*>& xceptions = xs->get_members();
      vector<t_field*>::const_iterator x_iter;
      for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
        f_service_ <<
          indent() << "if (result.__isset." << (*x_iter)->get_name() << ")"<< endl <<
          indent() << "then raise result." << prop_name(*x_iter) << endl <<
          indent() << "}" << endl;
      }

      if ((*f_iter)->get_returntype()->is_void()) {
        indent(f_service_) <<
          "return ()" << endl;
      } else {
        f_service_ <<
          indent() << "raise TApplicationException(TApplicationException.ExceptionType.MissingResult, \"" << (*f_iter)->get_name() << " failed: unknown result\")" << endl;
      }

      scope_down(f_service_);
      f_service_ << endl;
	  
    }
  }
  */
  indent_down();
  indent(f_service_);
}

void t_fsharp_generator::generate_service_server(t_service* tservice) {
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  string extends = "";
  string extends_processor = "";
  if (tservice->get_extends() != NULL) {
    extends = type_name(tservice->get_extends());
    extends_processor = extends + ".Processor, ";
  }

  indent(f_service_) <<
    "type Processor(iface:Iface) = " << endl <<
		extends_processor <<  "inherit TProcessor(iface) {" << endl;
  indent_up();

  f_service_ << endl;
  
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    f_service_ <<
      indent() << "let _Process:processMap_[\"" << (*f_iter)->get_name() << "\"] = " << (*f_iter)->get_name() << endl;
  }

  scope_down(f_service_);
  f_service_ << endl;

  if (extends.empty()) {
    f_service_ <<
      indent() << "let mutable ProcessFunction:int -> TProtocol -> TProtocol -> unit" << endl;
  }

  if (extends.empty()) {
    f_service_ <<
      indent() << "protected Dictionary<string, ProcessFunction> processMap_ = new Dictionary<string, ProcessFunction>();" << endl;
  }

  f_service_ << endl;

  if (extends.empty()) {
    indent(f_service_) <<
      "public bool Process(TProtocol iprot, TProtocol oprot)" << endl;
  }
  else
  {
    indent(f_service_) <<
      "public new bool Process(TProtocol iprot, TProtocol oprot)" << endl;
  }
  scope_up(f_service_);

  f_service_ <<  indent() << "try" << endl;
  scope_up(f_service_);

  f_service_ <<
    indent() << "TMessage msg = iprot.ReadMessageBegin();" << endl;

  f_service_ <<
    indent() << "ProcessFunction fn;" << endl <<
    indent() << "processMap_.TryGetValue(msg.Name, out fn);" << endl <<
    indent() << "if (fn == null) {" << endl <<
    indent() << "  TProtocolUtil.Skip(iprot, TType.Struct);" << endl <<
    indent() << "  iprot.ReadMessageEnd();" << endl <<
    indent() << "  TApplicationException x = new TApplicationException (TApplicationException.ExceptionType.UnknownMethod, \"Invalid method name: '\" + msg.Name + \"'\");" << endl <<
    indent() << "  oprot.WriteMessageBegin(new TMessage(msg.Name, TMessageType.Exception, msg.SeqID));" << endl <<
    indent() << "  x.Write(oprot);" << endl <<
    indent() << "  oprot.WriteMessageEnd();" << endl <<
    indent() << "  oprot.Transport.Flush();" << endl <<
    indent() << "  return true;" << endl <<
    indent() << "}" << endl <<
    indent() << "fn(msg.SeqID, iprot, oprot);" << endl;

  scope_down(f_service_);

  f_service_ <<
    indent() << "catch (IOException)" << endl;
  scope_up(f_service_);
  f_service_ <<
    indent() << "return false;" << endl;
  scope_down(f_service_);

  f_service_ <<
    indent() << "return true;" << endl;

  scope_down(f_service_);
  f_service_ << endl;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter)
  {
    generate_process_function(tservice, *f_iter);
  }

  indent_down();
  indent(f_service_) <<
    "}" << endl << endl;
}

void t_fsharp_generator::generate_function_helpers(t_function* tfunction) {
  if (tfunction->is_oneway()) {
    return;
  }

  t_struct result(program_, tfunction->get_name() + "_result");
  t_field success(tfunction->get_returntype(), "success", 0);
  if (!tfunction->get_returntype()->is_void()) {
    result.append(&success);
  }

  t_struct *xs = tfunction->get_xceptions();
  const vector<t_field*>& fields = xs->get_members();
  vector<t_field*>::const_iterator f_iter;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    result.append(*f_iter);
  }

  generate_fsharp_struct_definition(f_service_, &result, true, true);
}

void t_fsharp_generator::generate_process_function(t_service* tservice, t_function* tfunction) {
  (void) tservice;
  indent(f_service_) <<
    "public void " << tfunction->get_name() << "_Process(int seqid, TProtocol iprot, TProtocol oprot)" << endl;
  scope_up(f_service_);

  string argsname = tfunction->get_name() + "_args";
  string resultname = tfunction->get_name() + "_result";

  f_service_ <<
    indent() << argsname << " args = new " << argsname << "();" << endl <<
    indent() << "args.Read(iprot);" << endl <<
    indent() << "iprot.ReadMessageEnd();" << endl;

  t_struct* xs = tfunction->get_xceptions();
  const std::vector<t_field*>& xceptions = xs->get_members();
  vector<t_field*>::const_iterator x_iter;

  if (!tfunction->is_oneway()) {
    f_service_ <<
      indent() << resultname << " result = new " << resultname << "();" << endl;
  }

  if (xceptions.size() > 0) {
    f_service_ <<
      indent() << "try {" << endl;
    indent_up();
  }

  t_struct* arg_struct = tfunction->get_arglist();
  const std::vector<t_field*>& fields = arg_struct->get_members();
  vector<t_field*>::const_iterator f_iter;

  f_service_ << indent();
  if (!tfunction->is_oneway() && !tfunction->get_returntype()->is_void()) {
    f_service_ << "result.Success = ";
  }
  f_service_ <<
    "iface_." << tfunction->get_name() << "(";
  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
    } else {
      f_service_ << ", ";
    }
    f_service_ << "args." << prop_name(*f_iter);
  }
  f_service_ << ");" << endl;

  if (!tfunction->is_oneway() && xceptions.size() > 0) {
    indent_down();
    f_service_ << indent() << "}";
    for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
      f_service_ << " catch (" << type_name((*x_iter)->get_type(), false, false) << " " << (*x_iter)->get_name() << ") {" << endl;
      if (!tfunction->is_oneway()) {
        indent_up();
        f_service_ <<
          indent() << "result." << prop_name(*x_iter) << " = " << (*x_iter)->get_name() << ";" << endl;
        indent_down();
        f_service_ << indent() << "}";
      } else {
        f_service_ << "}";
      }
    }
    f_service_ << endl;
  }

  if (tfunction->is_oneway()) {
    f_service_ <<
      indent() << "return;" << endl;
    scope_down(f_service_);

    return;
  }

  f_service_ <<
    indent() << "oprot.WriteMessageBegin(new TMessage(\"" << tfunction->get_name() << "\", TMessageType.Reply, seqid)); " << endl <<
    indent() << "result.Write(oprot);" << endl <<
    indent() << "oprot.WriteMessageEnd();" << endl <<
    indent() << "oprot.Transport.Flush();" << endl;

  scope_down(f_service_);

  f_service_ << endl;
}

void t_fsharp_generator::generate_deserialize_field_inline(t_fs_ofstream& out, t_field* tfield) {
	t_type* type = tfield->get_type();
	while(type->is_typedef()) {
		type = ((t_typedef*)type)->get_type();
	}
	if (type->is_void()) { out << "()"; }

  
    if (type->is_struct() || type->is_xception()) {
      generate_deserialize_struct_inline(out, (t_struct*)type);
    } else if (type->is_container()) {
      generate_deserialize_container_inline(out, type);
    } else if (type->is_base_type() || type->is_enum()) {
      if (type->is_enum())
      {
        out << "enum<" << type_name(type, false, true) << ">";
      }
    
      out << "iprot.";
    
      if (type->is_base_type()) {
        t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
        switch (tbase) {
          case t_base_type::TYPE_VOID:
            throw "compiler error: cannot serialize void field in a struct: " + type->get_name();
            break;
          case t_base_type::TYPE_STRING:
            if (((t_base_type*)type)->is_binary()) {
               out << "ReadBinary()";
            } else {
              out << "ReadString()";
            }
            break;
          case t_base_type::TYPE_BOOL:
            out << "ReadBool()";
            break;
          case t_base_type::TYPE_BYTE:
            out << "ReadByte()";
            break;
          case t_base_type::TYPE_I16:
            out << "ReadI16()";
            break;
          case t_base_type::TYPE_I32:
            out << "ReadI32()";
            break;
          case t_base_type::TYPE_I64:
            out << "ReadI64()";
            break;
          case t_base_type::TYPE_DOUBLE:
            out << "ReadDouble()";
            break;
          default:
            throw "compiler error: no C# name for base type " + tbase;
        }
      } else if (type->is_enum()) {
        out << "ReadI32()";
      }
    } else {
      printf("DO NOT KNOW HOW TO DESERIALIZE FIELD '%s' TYPE '%s'\n", tfield->get_name().c_str(), type_name(type).c_str());
    }

}

void t_fsharp_generator::generate_deserialize_field(t_fs_ofstream& out, t_field* tfield, string prefix) {
  t_type* type = tfield->get_type();
  while(type->is_typedef()) {
    type = ((t_typedef*)type)->get_type();
  }

  if (type->is_void()) { out << "()"; }

  string name = prefix + prop_name(tfield);

  if (type->is_struct() || type->is_xception()) {
	out << name << " <- IsSet <| ";
    out.indent_up();
    generate_deserialize_struct_inline(out, (t_struct*)type);
    out << endl;
    out.indent_down();
  } else if (type->is_container()) {
      out << name << " <- IsSet <| ";
    generate_deserialize_container_inline(out, type);
  } else if (type->is_base_type() || type->is_enum()) {
    out << name << " <- IsSet <| ";

    if (type->is_enum())
    {
      out << "enum<" << type_name(type, false, true) << ">";
    }

    out << "iprot.";

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
      switch (tbase) {
        case t_base_type::TYPE_VOID:
          throw "compiler error: cannot serialize void field in a struct: " + name;
          break;
        case t_base_type::TYPE_STRING:
          if (((t_base_type*)type)->is_binary()) {
             out << "ReadBinary()";
          } else {
            out << "ReadString()";
          }
          break;
        case t_base_type::TYPE_BOOL:
          out << "ReadBool()";
          break;
        case t_base_type::TYPE_BYTE:
          out << "ReadByte()";
          break;
        case t_base_type::TYPE_I16:
          out << "ReadI16()";
          break;
        case t_base_type::TYPE_I32:
          out << "ReadI32()";
          break;
        case t_base_type::TYPE_I64:
          out << "ReadI64()";
          break;
        case t_base_type::TYPE_DOUBLE:
          out << "ReadDouble()";
          break;
        default:
          throw "compiler error: no F# name for base type " + tbase;
      }
    } else if (type->is_enum()) {
      out << "ReadI32()";
    }
    out << endl;
  } else {
    printf("DO NOT KNOW HOW TO DESERIALIZE FIELD '%s' TYPE '%s'\n", tfield->get_name().c_str(), type_name(type).c_str());
  }
}

void t_fsharp_generator::generate_deserialize_struct_inline(t_fs_ofstream& out, t_struct* tstruct) {
	out << "Helpers.readStruct iprot (" << type_name(tstruct) << "())";
}

void t_fsharp_generator::generate_deserialize_struct(t_fs_ofstream& out, t_struct* tstruct, string prefix) {
  out <<
    indent() << prefix << " = new " << type_name(tstruct) << "();" << endl <<
    indent() << "Helpers.readStruct iprot " << prefix << " |> ignore" << endl;
}

void t_fsharp_generator::generate_deserialize_container_inline(t_fs_ofstream& out, t_type* ttype) {
    string obj;

    if (ttype->is_map()) {
        obj = tmp("_map");
    }
    else if (ttype->is_set()) {
        obj = tmp("_set");
    }
    else if (ttype->is_list()) {
        obj = tmp("_list");
    }

    if (ttype->is_map()) {
        out << "Helpers.readMap iprot" << endl;
        out.indent_up();
        out.indent() << "(fun count -> " << endl;
        out.indent_up();
        out.indent() << "[ for i in 0 .. count - 1 do " << endl;
        out.indent_up();
        generate_deserialize_map_element(out, (t_map*)ttype);
        out << "] |> Map.ofList)";
        out.indent_down();
        out.indent_down();
        out.indent_down();
    }
    if (ttype->is_set()) {
        out << "Helpers.readSet iprot" << endl;
        out.indent_up();
        out.indent() << "(fun count -> " << endl;
        out.indent_up();
        out.indent() << "[ for i in 0 .. count - 1 do " << endl;
        out.indent_up();
        generate_deserialize_set_element(out, (t_set*)ttype);
        out << "] |> Set.ofList)";
        out.indent_down();
        out.indent_down();
        out.indent_down();
    }
    else if (ttype->is_list()) {
        out << "Helpers.readList iprot" << endl;
        out.indent_up();
        out.indent() << "(fun count -> " << endl;
        out.indent_up();
        out.indent() << "[ for i in 0 .. count - 1 do " << endl;
        out.indent_up();
        generate_deserialize_list_element(out, (t_list*)ttype);
        out.indent_down();
        out.indent_down();
        out.indent_down();        
    }
}

void t_fsharp_generator::generate_deserialize_container(t_fs_ofstream& out, t_type* ttype, string prefix) {
  string obj;

  if (ttype->is_set()) {
    obj = tmp("_set");    
  } else if (ttype->is_list()) {
    obj = tmp("_list");    
  }

  out << prefix << " = " << endl;
  out.indent_up();
  out.indent();
  generate_deserialize_container_inline(out, ttype);
  out.indent_down();
}

void t_fsharp_generator::generate_deserialize_map_element(t_fs_ofstream& out, t_map* tmap, string prefix) {
  string key = tmp("_key");
  string val = tmp("_val");

  t_field fkey(tmap->get_key_type(), key);
  t_field fval(tmap->get_val_type(), val);
  out.indent() << "yield ";
  generate_deserialize_field_inline(out, &fkey);
  out << ",";
  generate_deserialize_field_inline(out, &fval);
  out << "; ";
}

void t_fsharp_generator::generate_deserialize_set_element(t_fs_ofstream& out, t_set* tset, string prefix) {
  string elem = tmp("_elem");
  t_field felem(tset->get_elem_type(), elem);
  out << "yield ";
  generate_deserialize_field_inline(out, &felem);
  out << endl;
}

void t_fsharp_generator::generate_deserialize_list_element(t_fs_ofstream& out, t_list* tlist, string prefix) {
  string elem = tmp("_elem");
  t_field felem(tlist->get_elem_type(), elem);
  out << "yield ";
  generate_deserialize_field_inline(out, &felem);
  out << endl;
}

void t_fsharp_generator::generate_serialize_type(t_fs_ofstream& out, t_type* type, string name) {
    while (type->is_typedef()) {
        type = ((t_typedef*)type)->get_type();
    }

    if (type->is_void()) {
        throw "CANNOT GENERATE SERIALIZE CODE FOR void TYPE: " + name;
    }

    if (type->is_struct() || type->is_xception()) {
        generate_serialize_struct(out, (t_struct*)type, name);
    }
    else if (type->is_container()) {
        generate_serialize_container(out, type, name);
    }
    else if (type->is_base_type() || type->is_enum()) {

        out << "oprot.";

        if (type->is_base_type()) {
            t_base_type::t_base tbase = ((t_base_type*)type)->get_base();

            switch (tbase) {
            case t_base_type::TYPE_VOID:
                throw "compiler error: cannot serialize void field in a struct: " + name;
                break;
            case t_base_type::TYPE_STRING:
                if (((t_base_type*)type)->is_binary()) {
                    out << "WriteBinary(";
                }
                else {
                    out << "WriteString(";
                }
                out << name << ")";
                break;
            case t_base_type::TYPE_BOOL:
                out << "WriteBool(" << name << ")";
                break;
            case t_base_type::TYPE_BYTE:
                out << "WriteByte(" << name << ")";
                break;
            case t_base_type::TYPE_I16:
                out << "WriteI16(" << name << ")";
                break;
            case t_base_type::TYPE_I32:
                out << "WriteI32(" << name << ")";
                break;
            case t_base_type::TYPE_I64:
                out << "WriteI64(" << name << ")";
                break;
            case t_base_type::TYPE_DOUBLE:
                out << "WriteDouble(" << name << ")";
                break;
            default:
                throw "compiler error: no C# name for base type " + tbase;
            }
        }
        else if (type->is_enum()) {
            out << "WriteI32((int)" << name << ");";
        }
    }
    else {
        printf("DO NOT KNOW HOW TO SERIALIZE '%s' TYPE '%s'\n",
            name,
            type_name(type).c_str());
    }
}

void t_fsharp_generator::generate_serialize_value(t_fs_ofstream& out,t_field* tfield,string name) {
  t_type* type = tfield->get_type();
  out.indent();
  generate_serialize_type(out, type, name);
  out << endl;
}

void t_fsharp_generator::generate_serialize_field(t_fs_ofstream& out, t_field* tfield, string prefix) {
  std::string name = prefix + (tfield->get_name());
  generate_serialize_value(out,tfield,name);
}

void t_fsharp_generator::generate_serialize_struct(t_fs_ofstream& out, t_struct* tstruct, string prefix) {
  out << "Helpers.writeStruct oprot " << prefix << endl;
}

void t_fsharp_generator::generate_serialize_container(t_fs_ofstream& out, t_type* ttype, string prefix) {
  
 if (ttype->is_map()) {
      out << "oprot.WriteMapBegin(new TMap(" <<
          type_to_enum(((t_map*)ttype)->get_key_type()) << ", " <<
          type_to_enum(((t_map*)ttype)->get_val_type()) << ", " <<
          prefix << ".Count))" << endl;
      out.indent() << prefix << " |> Map.iter (fun k v -> " << endl;
      out.indent_up();
      out.indent();
      generate_serialize_type(out, ((t_map*)ttype)->get_key_type(),"k");
      out << endl;
      out.indent();
      generate_serialize_type(out, ((t_map*)ttype)->get_val_type(),"v");
      out << " )" << endl;
      out.indent_down();
      out.indent() << "oprot.WriteMapEnd()";
  } else if (ttype->is_set()) {
      out << "oprot.WriteSetBegin(new TSet(" <<
          type_to_enum(((t_set*)ttype)->get_elem_type()) << ", " <<
          prefix << ".Count))" << endl;
      out.indent() << prefix << " |> Seq.iter (fun v -> ";
      generate_serialize_type(out, ((t_set*)ttype)->get_elem_type(), "v");
      out << ")" << endl;
      out.indent() << "oprot.WriteSetEnd()";
  } else if (ttype->is_list()) {
      out << "oprot.WriteListBegin(new TList(" <<
          type_to_enum(((t_list*)ttype)->get_elem_type()) << ", " <<
          prefix << ".Count))" << endl;
      out.indent() << prefix << " |> List.iter (fun v -> ";
      generate_serialize_type(out, ((t_list*)ttype)->get_elem_type(), "v");
      out << ")" << endl;
      out.indent() << "oprot.WriteListEnd()";
  }
}

void t_fsharp_generator::generate_serialize_set_element(t_fs_ofstream& out, t_set* tset, string iter) {
  t_field efield(tset->get_elem_type(), iter);
  generate_serialize_field(out, &efield, "");
}

void t_fsharp_generator::generate_serialize_list_element(t_fs_ofstream& out, t_list* tlist, string iter) {
  t_field efield(tlist->get_elem_type(), iter);
  generate_serialize_field(out, &efield, "");
}

void t_fsharp_generator::generate_property(t_fs_ofstream& out, t_field* tfield, bool isPublic, bool generateIsset) {
    generate_fsharp_property(out, tfield, isPublic, generateIsset, "_");
}
void t_fsharp_generator::generate_fsharp_property(t_fs_ofstream& out, t_field* tfield, bool isPublic, bool generateIsset, std::string fieldPrefix) {
    out.indent() << (isPublic ? "public " : "private ") << type_name(tfield->get_type())
                << " " << prop_name(tfield) << endl;
    out.indent_up();
    out.indent() << "get" << endl;
    out.indent_up();
    out.indent() << "return " << fieldPrefix + tfield->get_name() << ";" << endl;
    out.indent_down();
    out.indent() << "set" << endl;
    out.indent_up();
    if (generateIsset) {
		out.indent() << "__isset." << tfield->get_name() << " = true;" << endl;
	}
    out.indent() << "this." << fieldPrefix + tfield->get_name() << " = value;" << endl;
    out.indent_down();
    out.indent_down();
    out << endl;
}

string t_fsharp_generator::type_name(t_type* ttype, bool in_container, bool in_init) {
  (void) in_init;
  while (ttype->is_typedef()) {
    ttype = ((t_typedef*)ttype)->get_type();
  }

  if (ttype->is_base_type()) {
    return base_type_name((t_base_type*)ttype, in_container);
  } else if (ttype->is_map()) {
    t_map *tmap = (t_map*) ttype;
    return "Map<" + type_name(tmap->get_key_type(), true) +
      ", " + type_name(tmap->get_val_type(), true) + ">";
  } else if (ttype->is_set()) {
    t_set* tset = (t_set*) ttype;
    return type_name(tset->get_elem_type(), true) + " Set";
  } else if (ttype->is_list()) {
    t_list* tlist = (t_list*) ttype;
    return type_name(tlist->get_elem_type(), true) + " list";
  }

  t_program* program = ttype->get_program();
  if (program != NULL && program != program_) {
    string ns = program->get_namespace("fsharp");
    if (!ns.empty()) {
      return ns + "." + ttype->get_name();
    }
  }

  return ttype->get_name();
}

string t_fsharp_generator::base_type_name(t_base_type* tbase, bool in_container) {
  (void) in_container;
  switch (tbase->get_base()) {
    case t_base_type::TYPE_VOID:
      return "void";
    case t_base_type::TYPE_STRING:
      if (tbase->is_binary()) {
        return "byte[]";
      } else {
        return "string";
      }
    case t_base_type::TYPE_BOOL:
      return "bool";
    case t_base_type::TYPE_BYTE:
      return "byte";
    case t_base_type::TYPE_I16:
      return "short";
    case t_base_type::TYPE_I32:
      return "int";
    case t_base_type::TYPE_I64:
      return "long";
    case t_base_type::TYPE_DOUBLE:
      return "double";
    default:
      throw "compiler error: no C# name for base type " + tbase->get_base();
  }
}

string t_fsharp_generator::declare_field(t_field* tfield, bool init, std::string prefix) {
  string result = "let " + prefix + tfield->get_name() + ":" + type_name(tfield->get_type());
  if (init) {
    t_type* ttype = tfield->get_type();
    while (ttype->is_typedef()) {
      ttype = ((t_typedef*)ttype)->get_type();
    }
    if (ttype->is_base_type() && tfield->get_value() != NULL) {
      t_fs_ofstream dummy;
      result += " = " + render_const_value(dummy, tfield->get_name(), ttype, tfield->get_value());
    } else if (ttype->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)ttype)->get_base();
      switch (tbase) {
        case t_base_type::TYPE_VOID:
          result += " = ()";
        case t_base_type::TYPE_STRING:
          result += " = \"\"";
          break;
        case t_base_type::TYPE_BOOL:
          result += " = false";
          break;
        case t_base_type::TYPE_BYTE:
		  result += " = 0u";
		  break;
        case t_base_type::TYPE_I16:
		  result += " = 0s";
        case t_base_type::TYPE_I32:
		  result += " = 0";
        case t_base_type::TYPE_I64:
          result += " = 0L";
          break;
        case t_base_type::TYPE_DOUBLE:
          result += " = 0.";
          break;
      }
    } else if (ttype->is_enum()) {
      result += " = enum 0";
    } else if (ttype->is_container()) {
      result += " = " + type_name(ttype, false, true) + "()";
    } else {
      result += " = " + type_name(ttype, false, true) + "()";
    }
  }
  return result + "\n";
}

string t_fsharp_generator::function_signature(t_function* tfunction, string prefix) {
  t_type* ttype = tfunction->get_returntype();
  return "member " + prefix + "." + tfunction->get_name() + "(" + argument_list(tfunction->get_arglist()) + "):"+type_name(ttype) + " =";
}


string t_fsharp_generator::argument_list(t_struct* tstruct) {
  string result = "";
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  bool first = true;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
    } else {
      result += ", ";
    }
    result += "(" + (*f_iter)->get_name() + ":" + type_name((*f_iter)->get_type()) + ")";
  }
  return result;
}

string t_fsharp_generator::type_to_enum(t_type* type) {
  while (type->is_typedef()) {
    type = ((t_typedef*)type)->get_type();
  }

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw "NO T_VOID CONSTRUCT";
      case t_base_type::TYPE_STRING:
        return "TType.String";
      case t_base_type::TYPE_BOOL:
        return "TType.Bool";
      case t_base_type::TYPE_BYTE:
        return "TType.Byte";
      case t_base_type::TYPE_I16:
        return "TType.I16";
      case t_base_type::TYPE_I32:
        return "TType.I32";
      case t_base_type::TYPE_I64:
        return "TType.I64";
      case t_base_type::TYPE_DOUBLE:
        return "TType.Double";
    }
  } else if (type->is_enum()) {
    return "TType.I32";
  } else if (type->is_struct() || type->is_xception()) {
    return "TType.Struct";
  } else if (type->is_map()) {
    return "TType.Map";
  } else if (type->is_set()) {
    return "TType.Set";
  } else if (type->is_list()) {
    return "TType.List";
  }

  throw "INVALID TYPE IN type_to_enum: " + type->get_name();
}

void t_fsharp_generator::generate_fsharp_docstring_comment(t_fs_ofstream &out, string contents) {
  generate_docstring_comment(out,
                             "/// <summary>\n",
                             "/// ", contents,
                             "/// </summary>\n");


}

void t_fsharp_generator::generate_fsharp_doc(t_fs_ofstream &out, t_field* field) {
  if (field->get_type()->is_enum()) {
    string combined_message = field->get_doc() + "\n<seealso cref=\"" + get_enum_class_name(field->get_type()) + "\"/>";
    generate_fsharp_docstring_comment(out, combined_message);
  } else {
    generate_fsharp_doc(out, (t_doc*)field);
  }
}

void t_fsharp_generator::generate_fsharp_doc(t_fs_ofstream &out, t_doc* tdoc) {
  if (tdoc->has_doc()) {
    generate_fsharp_docstring_comment(out, tdoc->get_doc());
  }
}

void t_fsharp_generator::generate_fsharp_doc(t_fs_ofstream &out, t_function* tfunction) {
  if (tfunction->has_doc()) {
	stringstream ps;
    const vector<t_field*>& fields = tfunction->get_arglist()->get_members();
    vector<t_field*>::const_iterator p_iter;
    for (p_iter = fields.begin(); p_iter != fields.end(); ++p_iter) {
      t_field* p = *p_iter;
      ps << "\n<param name=\"" << p->get_name() << "\">";
      if (p->has_doc()) {
		std::string str = p->get_doc();
		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end()); // remove the newlines that appear from the parser
		ps << str;
      }
	  ps << "</param>";
    }
    generate_docstring_comment(out,
                               "",
                               "/// ",
							   "<summary>\n" + tfunction->get_doc() + "</summary>" + ps.str(),
                               "");
  }
}

std::string t_fsharp_generator::get_enum_class_name(t_type* type) {
  string package = "";
  t_program* program = type->get_program();
  if (program != NULL && program != program_) {
    package = program->get_namespace("fsharp") + ".";
  }
  return package + type->get_name();
}

THRIFT_REGISTER_GENERATOR(fsharp, "F#","")

